/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.
*/

/*
 * File: rules.ld
 *
 * Linker Rules
 */

SECTIONS
{
  .hooks : ALIGN(16) SUBALIGN(16)
  {
    . = ALIGN(4);
    _hooks_start = .;
    KEEP(*(.hooks))
    . = ALIGN(4);
    _hooks_end = .;
  } > SRAM

  .init_array : ALIGN(4) SUBALIGN(4)
  {
    . = ALIGN(4);
    PROVIDE(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    . = ALIGN(4);
    PROVIDE(__init_array_end = .);
  } > SRAM

  .text : ALIGN(4) SUBALIGN(4)
  {
    . = ALIGN(4);
    _text_start = .;
    *(.text)
    *(.text.*)
    *(.glue_7)
    *(.glue_7t)
    *(.gcc*)
    . = ALIGN(4);
    _text_end = .;
  } > SRAM

  .rodata : ALIGN(4) SUBALIGN(4)
  {
    . = ALIGN(4);
    _rodata_start = .;
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _rodata_end = .;
  } > SRAM

  .data ALIGN(8) : ALIGN(8) SUBALIGN(8)
  {
    . = ALIGN(8);
    _data_start = .;
    *(.data)
    *(.data.*)
    . = ALIGN(8);
    _data_end = .;
  } > SRAM

  .bss (NOLOAD) : ALIGN(4)
  {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > SRAM

  .ARM.extab : ALIGN(4) SUBALIGN(4)
  {
    . = ALIGN(4);
    __extab_start = .;
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
    __extab_end = .;
  } > SRAM

  .ARM.exidx : ALIGN(4) SUBALIGN(4)
  {
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } > SRAM

  .eh_frame_hdr : ALIGN(4) SUBALIGN(4)
  {
    . = ALIGN(4);
    _eh_frame_hdr_start = .;
    *(.eh_frame_hdr)
    . = ALIGN(4);
    _eh_frame_hdr_end = .;
  } > SRAM

  .eh_frame : ALIGN(4) SUBALIGN(4) ONLY_IF_RO
  {
    . = ALIGN(4);
    _eh_frame_start = .;
    *(.eh_frame)
    . = ALIGN(4);
    _eh_frame_end = .;
  } > SRAM
}
