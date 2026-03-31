# QMK Userspace — Keebart Corne Choc Pro

## Setup

1. Use [qmk/qmk_userspace](https://github.com/qmk/qmk_userspace) as a template to create your repo
2. Clone your new repo
3. Unzip this into it
4. Add the Keebart fork as a submodule:

```bash
git submodule add https://github.com/Keebart/vial-qmk-corne-choc-pro.git qmk_firmware
git submodule update --init --recursive
```

5. Commit and push:

```bash
git add .
git commit -m "Initial keymap"
git push
```

6. GitHub Actions builds the .uf2 — download from the Releases tab
7. Flash: hold Q while plugging in left half, drag .uf2 onto RPI-RP2. Repeat with P for right.
