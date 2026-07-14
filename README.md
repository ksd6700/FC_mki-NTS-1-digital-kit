# FC_mki for KORG logue SDK synths

## English

FC_mki is a free, open-source user oscillator that brings compact Famicom-inspired tones to compatible KORG logue SDK synthesizers.

It is designed as a playable instrument rather than a cycle-accurate 2A03 emulator. The sound combines two pulse layers, an octave-down triangle layer, LFSR-style noise, sample-and-hold bit reduction, and a pulse sub oscillator. It works well for chiptune leads, basses, arpeggios, sound effects, and deliberately crunchy percussion.

### Supported instruments

| Instrument | Build target | Unit file |
| --- | --- | --- |
| KORG NTS-1 digital kit | `nutekt-digital` | `fc_mki.ntkdigunit` |
| KORG minilogue xd | `minilogue-xd` | `fc_mki.mnlgxdunit` |
| KORG prologue | `prologue` | `fc_mki.prlgunit` |

NTS-1 mkII, NTS-3 kaoss pad, microKORG 2, and drumlogue use a newer logue SDK unit API and are not supported by these builds yet.

### Controls

- `SHAPE`: pulse color and duty macro
- `SHIFT+SHAPE`: subtle pulse-2 detune
- `DUTY`: stepped 12.5%, 25%, 50%, or 75% pulse width
- `TRI`: octave-down triangle mix
- `NOISE`: noise mix and clock bite
- `CRUSH`: sample-and-hold and bit-depth grit
- `SUB`: octave-down pulse support
- `LEVEL`: output level

For a more console-like patch, turn delay and reverb off, use a short amp envelope, and play monophonic lines.

### Build

Clone or download [KORG's logue SDK](https://github.com/korginc/logue-sdk), install an ARM GNU toolchain, and point `LOGUE_SDK_DIR` at the SDK checkout.

Build one instrument:

```sh
make TARGET=nutekt-digital LOGUE_SDK_DIR=/path/to/logue-sdk install
make TARGET=minilogue-xd LOGUE_SDK_DIR=/path/to/logue-sdk install
make TARGET=prologue LOGUE_SDK_DIR=/path/to/logue-sdk install
```

Build all supported instruments:

```sh
make LOGUE_SDK_DIR=/path/to/logue-sdk all-platforms
```

The loadable units are written below `dist/<target>/`. Transfer the file for your instrument with the corresponding KORG Librarian application.

## 日本語

FC_mkiは、ファミコン風のコンパクトなサウンドをKORG logue SDK対応シンセで演奏するための、無料・オープンソースのユーザーオシレーターです。

2A03の厳密なエミュレーターではなく、楽器として弾きやすい音源として設計しています。2系統のパルス波、1オクターブ下の三角波、LFSR風ノイズ、サンプルホールド式のビットクラッシュ、サブパルスを組み合わせ、チップチューンのリード、ベース、アルペジオ、効果音、荒いパーカッションを作れます。

### 対応機種

| 機種 | ビルドターゲット | 読み込むファイル |
| --- | --- | --- |
| KORG NTS-1 digital kit | `nutekt-digital` | `fc_mki.ntkdigunit` |
| KORG minilogue xd | `minilogue-xd` | `fc_mki.mnlgxdunit` |
| KORG prologue | `prologue` | `fc_mki.prlgunit` |

NTS-1 mkII、NTS-3 kaoss pad、microKORG 2、drumlogueは新しいlogue SDK Unit APIを使うため、現時点ではこのビルドの対象外です。

### コントロール

- `SHAPE`: パルス波の音色とデューティ比をまとめて変化
- `SHIFT+SHAPE`: 2つ目のパルス波をわずかにデチューン
- `DUTY`: パルス幅を12.5%、25%、50%、75%から選択
- `TRI`: 1オクターブ下の三角波レベル
- `NOISE`: ノイズレベルとクロック感
- `CRUSH`: サンプルホールドとビット深度の荒さ
- `SUB`: 1オクターブ下のパルス波レベル
- `LEVEL`: 出力レベル

よりゲーム機らしく鳴らすには、ディレイとリバーブを切り、短いアンプEGで単音フレーズを演奏してください。

### ビルド

[KORG logue SDK](https://github.com/korginc/logue-sdk)とARM GNUツールチェーンを用意し、上の英語セクションにあるコマンドで対象機種を指定します。生成されたファイルは `dist/<target>/` に保存されます。各機種に対応するKORG Librarianを使って転送してください。

## Repository notes / リポジトリについて

This repository does not vendor the logue SDK. Linker and template files copied from KORG's BSD-licensed SDK are listed in `THIRD_PARTY_NOTICES.md`.

このリポジトリにはlogue SDK本体を同梱していません。KORGのBSDライセンスSDKから利用しているリンカーおよびテンプレートファイルは `THIRD_PARTY_NOTICES.md` に記載しています。
