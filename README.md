# FC_mki for KORG NTS-1 digital kit

FC_mki is a KORG NTS-1 / logue SDK user oscillator for compact Famicom-style tones.

It is not a cycle-accurate 2A03 emulator. It is a playable single-oscillator voice built from two pulse layers, an octave-down triangle layer, LFSR-style noise, and sample-hold bit reduction.

## Controls

- `SHAPE`: pulse color / duty macro
- `SHIFT+SHAPE`: subtle pulse-2 detune
- `DUTY`: stepped 12.5%, 25%, 50%, 75% pulse width selection
- `TRI`: octave-down triangle mix
- `NOISE`: noise mix and clock bite
- `CRUSH`: sample-hold and bit-depth grit
- `SUB`: octave-down pulse support
- `LEVEL`: output level

For the most console-like patch, keep delay/reverb off, use short amp EG settings, and play monophonic lines.

## Build

Clone or download KORG's logue SDK, then point `LOGUE_SDK_DIR` at it:

```sh
make LOGUE_SDK_DIR=/path/to/logue-sdk install
```

The loadable unit is written to:

```text
dist/fc_mki.ntkdigunit
```

Load `fc_mki.ntkdigunit` with the KORG Librarian / NTS-1 digital kit user oscillator transfer workflow.

## Repo Notes

This repository intentionally does not vendor the logue SDK. The copied linker/template files are from KORG's BSD-licensed SDK and are listed in `THIRD_PARTY_NOTICES.md`.

