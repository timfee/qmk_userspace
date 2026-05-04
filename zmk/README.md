# timfee ZMK config — beekeeb Toucan

Personal ZMK firmware config for the [beekeeb Toucan](https://beekeeb.com/toucan-keyboard/), a 42-key split with display and trackpad. Ported from my QMK userspace ([timfee/qmk_userspace](https://github.com/timfee/qmk_userspace)).

The shield definitions, `nice_view_gem`, trackpad wiring, and build scaffolding are unchanged from [beekeeb/zmk-keyboard-toucan](https://github.com/beekeeb/zmk-keyboard-toucan); only the keymap (`config/toucan.keymap`) is custom.

## Keymap

Three layers: BASE, SYM (numbers + shifted symbols), NAV (F-keys + arrow nav). Mod-tap thumbs and layer-tap pinkies. Same-hand chordal hold (always tap), cross-hand fast-typing protection via `require-prior-idle-ms`, balanced flavor with release-time evaluation.

## Build

GitHub Actions runs on push and produces `toucan_left.uf2`, `toucan_right.uf2`, and `settings_reset.uf2` under the workflow's Artifacts.

# License

The code in this repo is available under the MIT license.

The included shield nice_view_gem is modified from https://github.com/M165437/nice-view-gem licensed under the MIT License.

ZMK code snippets are taken from the ZMK documentation under the MIT license.

The embedded font QuinqueFive is designed by GGBotNet, licensed under under the SIL Open Font License, Version 1.1.
