ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    . = 0xC0100000;          /* the code should be relocated to 3GB + 1 MB */

    KERNEL_PHYSICAL_START = . - 0xC0000000;

    /* align at 4 KB and load at 1 MB, AT specifies load address */
    .text ALIGN (0x1000) : AT(ADDR(.text)-0xC0000000)
    {
        *(.text)             /* all text sections from all files */
    }

    /* align at 4 KB and load at 1 MB, AT specifies load address */
    .rodata ALIGN (0x1000) : AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    /* align at 4 KB and load at 1 MB, AT specifies load address */
    .data ALIGN (0x1000) : AT(ADDR(.data)-0xC0000000)
    {
        *(.data)             /* all data sections from all files */
    }

    /* align at 4 KB and load at 1 MB, AT specifies load address */
    .bss ALIGN (0x1000) : AT(ADDR(.bss)-0xC0000000)
    {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
    }

    KERNEL_PHYSICAL_END = . - 0xC0000000;
}
