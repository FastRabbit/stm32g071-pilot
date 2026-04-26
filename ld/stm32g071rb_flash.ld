/**
 * stm32g071rb_flash.ld
 *
 * Linker script for STM32G071RB:
 *   Flash: 128 KB  starting at 0x08000000
 *   SRAM:   36 KB  starting at 0x20000000
 */

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH =  36K
}

ENTRY(Reset_Handler)

SECTIONS
{
    /* ---- Interrupt vector table + code ---- */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        KEEP(*(.init))
        KEEP(*(.fini))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    /* LMA of .data — used by startup to copy */
    __data_start_flash = LOADADDR(.data);

    /* ---- Initialised data in SRAM ---- */
    .data :
    {
        __data_start = .;
        *(.data)
        *(.data.*)
        __data_end = .;
    } > RAM AT> FLASH

    /* ---- Zero-initialised data ---- */
    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    } > RAM

    /* ---- Stack (grows downward from top of RAM) ---- */
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        *(.stack)
    } > RAM

    /* ---- Heap ---- */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        *(.heap)
    } > RAM

    /* Symbols used by newlib _sbrk */
    end = .;
    _end = end;

    /* ---- Discard unwanted sections ---- */
    /DISCARD/ :
    {
        libc.a (*)
        libm.a (*)
        libgcc.a (*)
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
