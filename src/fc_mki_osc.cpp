#include "userosc.h"

namespace {

constexpr float kDefaultDuty = 0.50f;
constexpr float kDefaultTri = 0.18f;
constexpr float kDefaultNoise = 0.02f;
constexpr float kDefaultCrush = 0.36f;
constexpr float kDefaultSub = 0.12f;
constexpr float kDefaultLevel = 0.76f;

struct FcParams {
  float duty;
  float tri;
  float noise;
  float crush;
  float sub;
  float level;
  float shape;
  float shift_shape;

  float target_duty;
  float target_tri;
  float target_noise;
  float target_crush;
  float target_sub;
  float target_level;
  float target_shape;
  float target_shift_shape;
};

struct FcState {
  float phase_a;
  float phase_b;
  float sub_phase;
  float w0;
  float target_w0;
  float noise_clock;
  float noise_sample;
  float held_sample;
  float dc;
  float lp;
  uint32_t lfsr;
  uint32_t hold_countdown;
};

FcParams s_params;
FcState s_state;

inline float absf(float x) {
  return x < 0.0f ? -x : x;
}

inline float clamp(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

inline float clamp01(float x) {
  return clamp(x, 0.0f, 1.0f);
}

inline float lerp(float a, float b, float t) {
  return a + (b - a) * t;
}

inline float wrap01(float x) {
  while (x >= 1.0f) {
    x -= 1.0f;
  }
  while (x < 0.0f) {
    x += 1.0f;
  }
  return x;
}

inline float soft_clip(float x) {
  return x / (1.0f + absf(x) * 0.42f);
}

inline float safe_bl_idx(float idx) {
  return clamp(idx, 0.0f, 5.999f);
}

inline float note_float(uint16_t pitch) {
  return static_cast<float>(pitch >> 8) + static_cast<float>(pitch & 0xff) * (1.0f / 256.0f);
}

float duty_from_amount(float amount) {
  const float x = clamp01(amount);

  if (x < 0.25f) {
    return 0.125f;
  }

  if (x < 0.50f) {
    return 0.25f;
  }

  if (x < 0.75f) {
    return 0.50f;
  }

  return 0.75f;
}

float pulse_wave(float phase, float duty, float saw_idx) {
  const float p = wrap01(phase);
  const float shifted = wrap01(p + clamp(duty, 0.045f, 0.955f));
  const float a = osc_bl2_sawf(p, saw_idx);
  const float b = osc_bl2_sawf(shifted, saw_idx);
  return soft_clip((a - b) * 0.78f);
}

float triangle_wave(float phase) {
  const float p = wrap01(phase);
  return (p < 0.5f) ? (p * 4.0f - 1.0f) : (3.0f - p * 4.0f);
}

float next_noise(float clock) {
  s_state.noise_clock += clock;

  while (s_state.noise_clock >= 1.0f) {
    const uint32_t bit = (s_state.lfsr ^ (s_state.lfsr >> 1u)) & 1u;
    s_state.lfsr = (s_state.lfsr >> 1u) | (bit << 14u);
    s_state.lfsr &= 0x7fffu;
    s_state.noise_sample = (s_state.lfsr & 1u) ? 1.0f : -1.0f;
    s_state.noise_clock -= 1.0f;
  }

  return s_state.noise_sample;
}

float crush(float x, float amount) {
  const float crush_amount = clamp01(amount);
  const uint32_t hold = 1u + static_cast<uint32_t>(crush_amount * crush_amount * 10.0f);

  if (s_state.hold_countdown == 0u) {
    const float levels = 16.0f + (1.0f - crush_amount) * 112.0f;
    s_state.held_sample = si_roundf(clamp(x, -0.98f, 0.98f) * levels) / levels;
    s_state.hold_countdown = hold;
  } else {
    --s_state.hold_countdown;
  }

  return lerp(x, s_state.held_sample, crush_amount);
}

void set_defaults() {
  s_params.target_duty = kDefaultDuty;
  s_params.target_tri = kDefaultTri;
  s_params.target_noise = kDefaultNoise;
  s_params.target_crush = kDefaultCrush;
  s_params.target_sub = kDefaultSub;
  s_params.target_level = kDefaultLevel;
  s_params.target_shape = 0.50f;
  s_params.target_shift_shape = 0.22f;

  s_params.duty = s_params.target_duty;
  s_params.tri = s_params.target_tri;
  s_params.noise = s_params.target_noise;
  s_params.crush = s_params.target_crush;
  s_params.sub = s_params.target_sub;
  s_params.level = s_params.target_level;
  s_params.shape = s_params.target_shape;
  s_params.shift_shape = s_params.target_shift_shape;
}

void reset_state() {
  s_state.phase_a = 0.0f;
  s_state.phase_b = 0.25f;
  s_state.sub_phase = 0.0f;
  s_state.noise_clock = 0.0f;
  s_state.noise_sample = 1.0f;
  s_state.held_sample = 0.0f;
  s_state.dc = 0.0f;
  s_state.lp = 0.0f;
  s_state.lfsr = 1u;
  s_state.hold_countdown = 0u;
}

inline void slew_params() {
  constexpr float fast = 0.010f;
  constexpr float slow = 0.004f;

  s_params.duty += (s_params.target_duty - s_params.duty) * fast;
  s_params.tri += (s_params.target_tri - s_params.tri) * fast;
  s_params.noise += (s_params.target_noise - s_params.noise) * fast;
  s_params.crush += (s_params.target_crush - s_params.crush) * slow;
  s_params.sub += (s_params.target_sub - s_params.sub) * slow;
  s_params.level += (s_params.target_level - s_params.level) * slow;
  s_params.shape += (s_params.target_shape - s_params.shape) * fast;
  s_params.shift_shape += (s_params.target_shift_shape - s_params.shift_shape) * slow;
}

} // namespace

void OSC_INIT(uint32_t platform, uint32_t api) {
  (void)platform;
  (void)api;

  set_defaults();
  reset_state();
  s_state.w0 = osc_w0f_for_note(60, 0);
  s_state.target_w0 = s_state.w0;
}

void OSC_CYCLE(const user_osc_param_t * const params, int32_t *yn, const uint32_t frames) {
  q31_t * __restrict y = reinterpret_cast<q31_t *>(yn);

  const uint16_t pitch = params ? params->pitch : static_cast<uint16_t>(60u << 8);
  const float note = note_float(pitch);
  const float saw_idx = safe_bl_idx(osc_bl_saw_idx(note));

  s_state.target_w0 = osc_w0f_for_note(pitch >> 8, pitch & 0xff);

  const float shape_lfo = params ? q31_to_f32(params->shape_lfo) : 0.0f;
  const float panel_cutoff = params ? clamp01(static_cast<float>(params->cutoff) * (1.0f / 8191.0f)) : 0.75f;
  const float panel_reso = params ? clamp01(static_cast<float>(params->resonance) * (1.0f / 8191.0f)) : 0.0f;

  for (uint32_t i = 0; i < frames; ++i) {
    slew_params();

    s_state.w0 += (s_state.target_w0 - s_state.w0) * 0.0038f;
    const float detune = 1.0f + (s_params.shift_shape - 0.5f) * 0.0065f;

    s_state.phase_a = wrap01(s_state.phase_a + s_state.w0);
    s_state.phase_b = wrap01(s_state.phase_b + s_state.w0 * detune);
    s_state.sub_phase = wrap01(s_state.sub_phase + s_state.w0 * 0.5f);

    const float duty = duty_from_amount(s_params.duty * 0.64f + s_params.shape * 0.36f + shape_lfo * 0.08f);
    const float pulse_a = pulse_wave(s_state.phase_a, duty, saw_idx);
    const float pulse_b = pulse_wave(s_state.phase_b, duty_from_amount(s_params.shape + 0.18f), saw_idx);
    const float pulse = pulse_a * 0.72f + pulse_b * 0.28f;

    const float tri = triangle_wave(s_state.sub_phase);
    const float sub = pulse_wave(s_state.sub_phase, 0.50f, saw_idx);
    const float noise_clock = clamp(s_state.w0 * (18.0f + s_params.noise * 95.0f), 0.004f, 0.98f);
    const float noise = next_noise(noise_clock);

    float out = pulse * (0.86f - s_params.tri * 0.12f);
    out += tri * s_params.tri * 0.54f;
    out += sub * s_params.sub * 0.28f;
    out += noise * s_params.noise * 0.42f;

    s_state.dc += (out - s_state.dc) * 0.0007f;
    out -= s_state.dc;

    const float tone = clamp01(0.42f + panel_cutoff * 0.50f - s_params.crush * 0.18f);
    const float lp_coef = clamp(0.045f + tone * tone * 0.50f + panel_reso * 0.025f, 0.035f, 0.62f);
    s_state.lp += (out - s_state.lp) * lp_coef;
    out = s_state.lp + (out - s_state.lp) * (0.28f + tone * 0.88f);

    out = crush(soft_clip(out * (1.05f + s_params.crush * 0.28f)), s_params.crush);
    out = clamp(out * s_params.level, -0.96f, 0.96f);

    y[i] = f32_to_q31(out);
  }
}

void OSC_NOTEON(const user_osc_param_t * const params) {
  reset_state();

  if (params) {
    s_state.w0 = osc_w0f_for_note(params->pitch >> 8, params->pitch & 0xff);
    s_state.target_w0 = s_state.w0;
  }
}

void OSC_NOTEOFF(const user_osc_param_t * const params) {
  (void)params;
}

void OSC_MUTE(const user_osc_param_t * const params) {
  (void)params;
  reset_state();
}

void OSC_PARAM(uint16_t index, uint16_t value) {
  const float percent = clamp01(static_cast<float>(value) * 0.01f);

  switch (index) {
  case k_user_osc_param_id1:
    s_params.target_duty = percent;
    break;

  case k_user_osc_param_id2:
    s_params.target_tri = percent;
    break;

  case k_user_osc_param_id3:
    s_params.target_noise = percent;
    break;

  case k_user_osc_param_id4:
    s_params.target_crush = percent;
    break;

  case k_user_osc_param_id5:
    s_params.target_sub = percent;
    break;

  case k_user_osc_param_id6:
    s_params.target_level = 0.04f + percent * 0.92f;
    break;

  case k_user_osc_param_shape:
    s_params.target_shape = clamp01(param_val_to_f32(value));
    break;

  case k_user_osc_param_shiftshape:
    s_params.target_shift_shape = clamp01(param_val_to_f32(value));
    break;

  default:
    break;
  }
}
