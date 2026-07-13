/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.
*/

/*
 * File: userosc.ld
 *
 * Linker Script for user oscillators
 */

ENTRY(_entry)

MEMORY
{
  SRAM   (rx) : org = 0x20000000, len = 32K
}

INCLUDE rules.ld
