#include "kheap.h"

u32int g_CurrentVirtualAddressTop = 0;

void set_physical_address_top(u32int phyAddress) {
  g_CurrentPhysicalAddressTop = (u32int)&phyAddress;
}

 u32int kmalloc_int(u32int size, u32int align, u32int *pAddrPtr) {
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

u32int kmalloc_a(u32int size) { return kmalloc_int(size, 1, 0); }

u32int kmalloc_p(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 0, pAddrPtr);
}

u32int kmalloc_ap(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 1, pAddrPtr);
}

u32int kmalloc(u32int size) { return kmalloc_int(size, 0, 0); }
