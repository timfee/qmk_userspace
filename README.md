# QMK Userspace — Keebart Corne Choc Pro

Self-contained repo: firmware (Keebart's QMK fork) is embedded as a submodule at `qmk_firmware/`.
No waiting on upstream — cherry-pick, merge, or patch anything you want right here.

## Quick start

```bash
git clone --recursive https://github.com/timfee/qmk_userspace.git
cd qmk_userspace
```

GitHub Actions builds the `.uf2` automatically on push — download from the **Releases** tab.

Flash: hold **Q** while plugging in the left half, drag `.uf2` onto `RPI-RP2`. Repeat with **P** for the right half.

## Local builds

```bash
pip install qmk
qmk setup -H qmk_firmware   # point QMK at the submodule
qmk compile -kb keebart/corne_choc_pro/standard -km timfee
```

## Pulling upstream changes

The submodule tracks the `vial` branch of `Keebart/vial-qmk-corne-choc-pro`.

```bash
# Pull latest from Keebart
cd qmk_firmware
git fetch origin
git merge origin/vial        # or: git rebase origin/vial
cd ..
git add qmk_firmware
git commit -m "Update firmware submodule"
```

### Cherry-picking from other repos (e.g. Keebart/vial-qmk-keebart, qmk/qmk_firmware)

```bash
cd qmk_firmware

# Add any upstream remote once
git remote add upstream-qmk https://github.com/qmk/qmk_firmware.git
git remote add keebart-full https://github.com/Keebart/vial-qmk-keebart.git

# Fetch and cherry-pick
git fetch keebart-full
git cherry-pick <commit-sha>   # e.g. OLED SSD1312 support

cd ..
git add qmk_firmware
git commit -m "Cherry-pick: <description>"
```

### Switching to your own firmware fork

If you fork `Keebart/vial-qmk-corne-choc-pro` to your own GitHub account:

```bash
cd qmk_firmware
git remote set-url origin https://github.com/YOUR_USER/vial-qmk-corne-choc-pro.git
git push origin vial          # push your changes to your fork
cd ..
# Update .gitmodules to point at your fork
git config -f .gitmodules submodule.qmk_firmware.url \
  https://github.com/YOUR_USER/vial-qmk-corne-choc-pro.git
git add .gitmodules qmk_firmware
git commit -m "Point submodule at my firmware fork"
```
