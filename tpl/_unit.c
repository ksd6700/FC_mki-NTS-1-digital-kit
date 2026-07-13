/*
    BSD 3-Clause License

    Copyright (c) 2018, KORG INC.
    All rights reserved.
*/

/**
 * @file    _unit.c
 * @brief   Oscillator entry template.
 */

#include <stddef.h>

#include "userosc.h"

extern uint8_t _bss_start;
extern uint8_t _bss_end;

extern void (*__init_array_start []) (void);
extern void (*__init_array_end []) (void);

typedef void (*__init_fptr)(void);

__attribute__((used, section(".hooks")))
static const user_osc_hook_table_t s_hook_table = {
  .magic = {'U','O','S','C'},
  .api = USER_API_VERSION,
  .platform = USER_TARGET_PLATFORM>>8,
  .reserved0 = {0},
  .func_entry = _entry,
  .func_cycle = _hook_cycle,
  .func_on = _hook_on,
  .func_off = _hook_off,
  .func_mute = _hook_mute,
  .func_value = _hook_value,
  .func_param = _hook_param,
  .reserved1 = {0}
};

__attribute__((used))
void _entry(uint32_t platform, uint32_t api)
{
  uint8_t * __restrict bss_p = (uint8_t *)&_bss_start;
  const uint8_t * const bss_e = (uint8_t *)&_bss_end;

  for (; bss_p != bss_e;)
    *(bss_p++) = 0;

  const size_t count = __init_array_end - __init_array_start;
  for (size_t i = 0; i < count; ++i) {
    __init_fptr init_p = (__init_fptr)__init_array_start[i];
    if (init_p != NULL)
      init_p();
  }

  _hook_init(platform, api);
}

__attribute__((weak))
void _hook_init(uint32_t platform, uint32_t api)
{
  (void)platform;
  (void)api;
}

__attribute__((weak))
void _hook_cycle(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames)
{
  (void)params;
  (void)yn;
  (void)frames;
}

__attribute__((weak))
void _hook_on(const user_osc_param_t * const params)
{
  (void)params;
}

__attribute__((weak))
void _hook_off(const user_osc_param_t * const params)
{
  (void)params;
}

__attribute__((weak))
void _hook_mute(const user_osc_param_t * const params)
{
  (void)params;
}

__attribute__((weak))
void _hook_value(uint16_t value)
{
  (void)value;
}

__attribute__((weak))
void _hook_param(uint16_t index, uint16_t value)
{
  (void)index;
  (void)value;
}
