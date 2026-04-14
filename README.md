# QMK Userspace — Keebart Corne Choc Pro

QMK userspace overlay for the Keebart Corne Choc Pro. Builds against
[Keebart/vial-qmk-corne-choc-pro](https://github.com/Keebart/vial-qmk-corne-choc-pro) (vial branch).

GitHub Actions builds the `.uf2` automatically on push — download from the **Releases** tab.

Flash: hold **Q** while plugging in the left half, drag `.uf2` onto `RPI-RP2`. Repeat with **P** for the right half.

## Local builds

```bash
pip install qmk
qmk setup -H . --home $(pwd)/qmk_firmware
qmk compile -kb keebart/corne_choc_pro/standard -km timfee
```
