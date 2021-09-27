#include "kheap.h"
#include "../../utils/array/ordered_array.h"
#include "../paging/paging.h"

typedef struct {
  /* Array of holes sorted by size (ordered array) */
  ordered_array_t index;
  /* The start of heap. */
  u32int startAddress;
  /* The end of heap. Can be expanded up to maxAddress. */
  u32int endAddress;
  /* The maximum address the heap can be expanded to. */
  u32int maxAddress;
  /* If set, accesible only in supervisor mode */
  u8int supervisor;
  /* Page permission (if set, read only) */
  u8int readonly;
} heap_t;

typedef struct {
  u32int magic; // Magic number, used for error checking and identification.
  u8int isHole; // 1 if this is a hole. 0 if this is a block.
  u32int size;  // size of the block, including the end footer.
} header_t;

typedef struct {
  u32int magic;     // Magic number, same as in header_t.
  header_t *header; // Pointer to the block header.
} footer_t;

u32int g_CurrentPhysicalAddressTop = 0;
heap_t *g_KernelHeap = 0;

/*
 * Defined in paging.c (using extern for the sake of security, so that these are
 * not exposed in header)
 */
extern page_directory_t *g_kernelDirectory;

void set_physical_address_top(u32int kernelPhysicalEnd) {
  g_CurrentPhysicalAddressTop = kernelPhysicalEnd;
}

static s32int find_smallest_hole(u32int size, u8int pageAlign, heap_t *heap) {
  /* Find the smallest hole that will fit. */
  u32int iterator = 0;

  while (iterator < heap->index.size) {
    header_t *header = (header_t *)peek_ordered_array(iterator, &heap->index);

    /* If the user has requested the memory be page-aligned */
    if (pageAlign > 0) {
      /* Page-align the starting point of this header. */
      u32int location = (u32int)header;
      s32int offset = 0;

      if (((location + sizeof(header_t)) & 0xFFFFF000) != 0) {
        offset = 0x1000 - ((location + sizeof(header_t)) % 0x1000);
      }

      s32int holeSize = (s32int)header->size - offset;

      /* Can we fit now? */
      if (holeSize >= (s32int)size)
        break;
    } else if (header->size >= size) {
      break;
    }
    iterator++;
  }

  /* We got to the end and didn't find anything. */
  if (iterator == heap->index.size) {
    return -1;
  } else {
    return iterator;
  }
}

static void expand(u32int newSize, heap_t *heap) {
  if (newSize <= heap->endAddress - heap->startAddress) {
  
   // print_screen("\nError: Expand: New Size is not greater than old size!!!\n");
    return;
  }

  /* Get the nearest following page boundary. */
  if (newSize & 0x00000FFF) {
    newSize &= 0xFFFFF000;
    newSize += 0x1000;
  }

  /* Make sure we are not overreaching ourselves. */
  if (heap->startAddress + newSize > heap->maxAddress) {
    //print_screen("\nError: Expand: New Size is greater than max size!!!\n");
    return;
  }

  u32int oldSize = heap->endAddress - heap->startAddress;

  u32int index = oldSize;
  /* Allocate frames for new memory area */
  while (index < newSize) {
    alloc_frame(get_page(heap->startAddress + index, 1, g_kernelDirectory),
                (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
    index += 0x1000;
  }
  heap->endAddress = heap->startAddress + newSize;
}

type_t alloc(u32int size, u8int pageAlign, heap_t *heap) {

  /* Make sure we take the size of header/footer into account. */
  u32int newSize = size + sizeof(header_t) + sizeof(footer_t);

  /* Find the smallest hole that will fit. */
  s32int iterator = find_smallest_hole(newSize, pageAlign, heap);

  /* If we didn't find a suitable hole */
  if (iterator == -1) {
    /* Save previous data. */
    u32int oldLength = heap->endAddress - heap->startAddress;
    u32int oldEndAddress = heap->endAddress;

    /* We need to allocate some more space. */
    expand(oldLength + newSize, heap);
    u32int newLength = heap->endAddress - heap->startAddress;

    /*
     * Since there is not hole available and we have now expanded our heap, we
     * need either add the new hole to list of available holes or create a new
     * entry on sorted array of holes if this is first insertion
     */
    iterator = 0;
    s32int idx = -1;
    u32int value = 0x0;

    u32int tmp =
        (u32int)peek_ordered_array((heap->index.size - 1), &heap->index);
    if (tmp > value) {
      value = tmp;
      idx = iterator;
    }

    /* If we didn't find ANY headers, we need to add one. */
    if (idx == -1) {
      header_t *header = (header_t *)oldEndAddress;
      header->magic = HEAP_MAGIC;
      header->size = newLength - oldLength;
      header->isHole = 1;
      footer_t *footer =
          (footer_t *)(oldEndAddress + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
      insert_ordered_array((type_t)header, &heap->index);
    } else {
      /*
       * This will probably be the last header that has size less han required
       * and hence needs adjusting.
       */
      header_t *header = peek_ordered_array(idx, &heap->index);
      header->size += newLength - oldLength;
      // Rewrite the footer.
      footer_t *footer =
          (footer_t *)((u32int)header + header->size - sizeof(footer_t));
      footer->header = header;
      footer->magic = HEAP_MAGIC;
    }
    /*
     * We now have enough space. Recurse, and call the function again, so that
     * in the next iteration we go and do the book keeping things like boundary
     * check and removing the block from holes etc...
     */
    return alloc(size, pageAlign, heap);
  }

  /* If we find the smallest hole that will fit */
  header_t *origHoleHeader =
      (header_t *)peek_ordered_array(iterator, &heap->index);
  u32int origHolePos = (u32int)origHoleHeader;
  u32int origHoleSize = origHoleHeader->size;

  /*
   * Here we work out if we should split the hole we found into two parts.
   * We will skip splitting the hole into two if the original hole size -
   * requested hole size less than the overhead for adding a new hole as we will
   * not have any space left to allocate in the new block.
   */
  if ((origHoleSize - newSize) <= (sizeof(header_t) + sizeof(footer_t))) {
    size += origHoleSize - newSize;
    newSize = origHoleSize;
  }

  /*
   * If we need to page-align the data, do it now and make a new hole before
   * our new block.
   */
  if (pageAlign && (origHolePos & 0x00000FFF)) {
    u32int newLocation =
        origHolePos + 0x1000 - (origHolePos & 0xFFF) - sizeof(header_t);

    /* If left over space has at least 1 byte add new hole */
    u32int residualSize = 0x1000 - (origHolePos & 0xFFF) - sizeof(header_t);
    if ((origHoleSize - residualSize) > (sizeof(header_t) + sizeof(footer_t))) {
      header_t *holeHeader = (header_t *)origHolePos;
      holeHeader->size = residualSize;
      holeHeader->magic = HEAP_MAGIC;
      holeHeader->isHole = 1;
      footer_t *holeFooter =
          (footer_t *)((u32int)newLocation - sizeof(footer_t));
      holeFooter->magic = HEAP_MAGIC;
      holeFooter->header = holeHeader;
      insert_ordered_array((type_t)holeHeader, &heap->index);
    } else {
      /* TODO: Implement logic to expand block on left to avoid fragmentation */
    }

    origHolePos = newLocation;
    origHoleSize = origHoleSize - residualSize;
  } else {
    /*
     * Else we don't need this hole any more as we will be allocating it, delete
     * it from the index.
     */
    remove_ordered_array(iterator, &heap->index);
  }

  /*
   * Create header and footer if this is newly allocated block or Overwrite the
   * original if we expanded the last hole.
   */
  header_t *blockHeader = (header_t *)origHolePos;
  blockHeader->magic = HEAP_MAGIC;
  blockHeader->isHole = 0;
  blockHeader->size = newSize;
  footer_t *blockFooter = (footer_t *)(origHolePos + sizeof(header_t) + size);
  blockFooter->magic = HEAP_MAGIC;
  blockFooter->header = blockHeader;

  /*
   * We may need to write a new hole after the allocated block.
   * We do this only if the new hole would have positive size...
   */
  if (origHoleSize - newSize > 0) {
    header_t *holeHeader =
        (header_t *)(origHolePos + sizeof(header_t) + size + sizeof(footer_t));
    holeHeader->magic = HEAP_MAGIC;
    holeHeader->isHole = 1;
    holeHeader->size = origHoleSize - newSize;
    footer_t *holeFooter = (footer_t *)((u32int)holeHeader + origHoleSize -
                                        newSize - sizeof(footer_t));
    if ((u32int)holeFooter < heap->endAddress) {
      holeFooter->magic = HEAP_MAGIC;
      holeFooter->header = holeHeader;
    }
    /*
     * Put the new hole in the index. If the footer is beyond the heap end
     * address we will still add the hole to index so that we can expand the
     * last hole without fragmentation.
     */
    insert_ordered_array((type_t)holeHeader, &heap->index);
  }

  return (type_t)((u32int)blockHeader + sizeof(header_t));
}

u32int kmalloc_int(u32int size, u32int align, u32int *pAddrPtr) {
  /* if Kernel Heap is created and allocated */
  if (g_KernelHeap != 0) {
    type_t addr = alloc(size, (u8int)align, g_KernelHeap);
    if (pAddrPtr != 0) {
      page_t *page = get_page((u32int)addr, 0, g_kernelDirectory);
      if ((u32int)addr & 0x00000FFF) {
       // print_screen("\nError: kmalloc_int: returned address from alloc is not page aligned!!!!\n");
      }
      *pAddrPtr = page->frame * 0x1000;
    }
    return (u32int)addr;
  } else {
    if (align == 1) {
      if (g_CurrentPhysicalAddressTop & 0x00000FFF) {
        // Align the placement address;
        g_CurrentPhysicalAddressTop &= 0xFFFFF000;
        g_CurrentPhysicalAddressTop += 0x1000;
      }
    }
    if (pAddrPtr) {
      *pAddrPtr = g_CurrentPhysicalAddressTop;
    }
    u32int tmp = g_CurrentPhysicalAddressTop;
    g_CurrentPhysicalAddressTop += size;
    return tmp;
  }
}

u32int kmalloc_a(u32int size) { return kmalloc_int(size, 1, 0); }

u32int kmalloc_p(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 0, pAddrPtr);
}

u32int kmalloc_ap(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 1, pAddrPtr);
}

u32int kmalloc(u32int size) { return kmalloc_int(size, 0, 0); }

/*
 * Compare function for ordered array, sorted by header size ascending order
 */
static u8int header_t_less_than(type_t a, type_t b) {
  return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t *create_heap(u32int startAddr, u32int endAddr, u32int maxAddr,
                    u8int supervisor, u8int readonly) {
  heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

  if (startAddr % 0x1000 != 0) {
    //print_screen("\nError: create_heap: Start Address is not page aligned\n");
    return 0;
  }

  if (endAddr % 0x1000 != 0) {
    //print_screen("\nError: create_heap: End Address is not page aligned\n");
    return 0;
  }

  /* Initialise the index. */
  heap->index = place_ordered_array((type_t)startAddr, HEAP_INDEX_SIZE,(compare_predicate_t)&header_t_less_than);

  /*
   * Shift the start address forward to resemble where we can start putting
   * data.
   */
  startAddr += sizeof(type_t) * HEAP_INDEX_SIZE;

  /* Make sure the start address is page-aligned. */
  if (startAddr & 0x00000FFF) {
    startAddr &= 0xFFFFF000;
    startAddr += 0x1000;
  }

  /* Write the start, end and max addresses into the heap structure. */
  heap->startAddress = startAddr;
  heap->endAddress = endAddr;
  heap->maxAddress = maxAddr;
  heap->supervisor = supervisor;
  heap->readonly = readonly;

  /* We start off with one large hole in the index. */
  header_t *hole = (header_t *)startAddr;
  hole->size = endAddr - startAddr;
  hole->magic = HEAP_MAGIC;
  hole->isHole = 1;
  insert_ordered_array((type_t)hole, &heap->index);

  return heap;
}

void create_kernel_heap(u32int startAddr, u32int endAddr, u32int maxAddr) {
  g_KernelHeap = create_heap(startAddr, endAddr, maxAddr, 0, 0);
}

static u32int contract(u32int newSize, heap_t *heap) {
  if (newSize >= (heap->endAddress - heap->startAddress)) {
    //print_screen("\nError: contract: New size greater than current heap size");
    return (heap->endAddress - heap->startAddress);
  }

  /* Page align */
  if (newSize & 0x0FFF) {
    newSize &= 0x1000;
    newSize += 0x1000;
  }

  /* Don't contract too far! */
  if (newSize < HEAP_MIN_SIZE)
    newSize = HEAP_MIN_SIZE;

  u32int oldSize = heap->endAddress - heap->startAddress;
  u32int index = oldSize - 0x1000;
  while (newSize < index) {
    free_frame(get_page(heap->startAddress + index, 0, g_kernelDirectory));
    index -= 0x1000;
  }

  heap->endAddress = heap->startAddress + newSize;
  return newSize;
}

static void free(void *ptr, heap_t *heap) {
  if (ptr == 0) {
    return;
  }

  header_t *header = (header_t *)((u32int)ptr - sizeof(header_t));
  footer_t *footer =
      (footer_t *)((u32int)header + header->size - sizeof(footer_t));

  if (header->magic != HEAP_MAGIC) {
    //print_screen("\nError: free: Cannot verify header magic numer");
  }

  if (footer->magic != HEAP_MAGIC) {
    //print_screen("\nError: free: Cannot verify footer magic number");
  }

  /* Make us a hole */
  header->isHole = 1;

  /* We want to add this header into the 'free holes' index */
  char doAdd = 1;

  /*
   * Unify left if the thing immediately to the left of us is a footer of free
   * block
   */
  footer_t *testFooter = (footer_t *)((u32int)header - sizeof(footer_t));
  if (testFooter->magic == HEAP_MAGIC && testFooter->header->isHole == 1) {
    u32int cacheSize = header->size;
    header = testFooter->header;
    footer->header = header;
    header->size += cacheSize;
    /* Since this header is already in the index, we don't want to add */
    doAdd = 0;
  }

  /*
   * Unify right if the thing immediately to the right of us is a header of a
   * free block
   */
  header_t *testHeader = (header_t *)((u32int)footer + sizeof(footer_t));
  if (testHeader->magic == HEAP_MAGIC && testHeader->isHole) {
    header->size += testHeader->size;
    testFooter =
        (footer_t *)((u32int)testHeader + testHeader->size - sizeof(footer_t));
    footer = testFooter;
    /*  Find and remove this header from the index. */
    u32int iterator = 0;
    while ((iterator < heap->index.size) &&
           (peek_ordered_array(iterator, &heap->index) != (type_t)testHeader)) {
      iterator++;
    }

    if (iterator >= heap->index.size) {
      //print_screen("\nError: free: unify right found a free block that is not present in index");
      return;
    }

    remove_ordered_array(iterator, &heap->index);
  }

  /*
   * If the footer location is the end address, we can contract if we have
   * expanded our heap beyond minimum heap size.
   */
  if ((u32int)footer + sizeof(footer_t) == heap->endAddress) {
    u32int oldLength = heap->endAddress - heap->startAddress;
    u32int newLength = contract((u32int)header - heap->startAddress, heap);

    /*
     * Check how big we will be after resizing.
     * It might be possible we still have the header but footer stays outside
     * heap end address since we contracted
     */
    if (header->size - (oldLength - newLength) > 0) {
      header->size -= oldLength - newLength;
      footer = (footer_t *)((u32int)header + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
    } else {
      /* Remove temporary header before contract from the index. */
      u32int iterator = 0;
      while (
          (iterator < heap->index.size) &&
          (peek_ordered_array(iterator, &heap->index) != (type_t)testHeader)) {
        iterator++;
      }

      if (iterator >= heap->index.size) {
        //print_screen("\nError: free: contract found a free block that is not present in index");
        return;
      }

      if (iterator < heap->index.size) {
        remove_ordered_array(iterator, &heap->index);
      }
    }
  }

  /* If required, add us to the index. */
  if (doAdd == 1)
    insert_ordered_array((void *)header, &heap->index);
}

void kfree(void *ptr) { free(ptr, g_KernelHeap); }
