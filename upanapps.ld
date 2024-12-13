/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the app binary */
SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x40000000)); . = SEGMENT_START("text-segment", 0x40000000) + SIZEOF_HEADERS;
/*  . = 0x40000000;*/

  .text : ALIGN(4K)
  {
    *(.text)
    *(.text*)
  }

  /* Read-only data. */
  .rodata : ALIGN(32)
  {
    *(.rodata*)
  }

  /* Read-write data (initialized) */
  .data : ALIGN(32)
  {
    *(.data*)
  }

  /* Read-write data (uninitialized) and stack */
  .bss : ALIGN(32)
  {
    *(COMMON)
    *(.bss*)
  }

  /* Just make crtstuff happy */
  .tm_clone_table :
  {
    *(.tm_clone_table)
  }
  .jcr :
  {
    *(.jcr)
  }
  .got.plt : 
  {
    *(.got.plt)
  }

  .init : ALIGN(16)
  {
    *(.init)
  }

  .fini : ALIGN(16)
  {
    *(.fini)
  }

  /* Global ctors and dtors */
  .ctors : ALIGN(8)
  {
    *(.ctors*)
  }

  .dtors : ALIGN(8)
  {
    *(.dtors*)
  }

  .eh_frame_hdr : ALIGN(8)
  {
    *(.eh_frame_hdr*)
  }

  .eh_frame : ALIGN(8)
  {
    *(.eh_frame*)
  }

  .gcc_except_table : ALIGN(8)
  {
    *(.gcc_except_table*)
  }

  .rela.plt : ALIGN(8)
  {
    *(.rela.plt*)
  }

  .tdata : ALIGN(8)
  {
    *(.tdata*)
  }

  .tbss : ALIGN(8)
  {
    *(.tbss*)
  }

  .debug_info  : { *(.debug_info*) }
  .debug_abbrev  : { *(.debug_abbrev*) }
  .debug_aranges  : { *(.debug_aranges*) }
  .debug_ranges  : { *(.debug_ranges*) }
  .debug_line  : { *(.debug_line*) }
  .debug_str  : { *(.debug_str*) }

  .shstrtab : { *(.shstrtab*) }
  .symtab  : { *(.symtab*) }
  .strtab  : { *(.strtab*) }

  /* The compiler may produce other sections, by default it will put them in
     a segment with the same name. Simply add stuff here as needed. */
}
