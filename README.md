# Onewheel Firmware Extractor

Onewheel OTA firmware that extracts keys and settings from your board.

## Getting started

This firmware is pre-compiled and available to use in [rewheel.app](https://rewheel.app), which will let you dump your boards keys, settings, and a portion of it's internal firmware directly over bluetooth.

## Build instructions

### Prerequisites

- PlatformIO - You can install via VS Code as an extension (recommended) or install via Python Pip. For the latter, You'll need to have the latest version of Python installed. Afterwards, you can use `pip` to install `PlatformIO` which is used to build the project.
```sh
pip3 install platformio
```

### Clone and build

To get the project going you'll need to clone and build the latest version of the repo. Once you've cloned it, building the pint binary is as simple as running `pio run -e pint` (or `pio run -e xr` for XR).

```sh
git clone git@github.com:exPHAT/ow-firmware-extractor.git
cd ow-firmware-extractor
pio run -e pint
```

That will write a compiled copy of firmware at `.pio/build/pint/firmware.bin` (`.pio/build/xr/firmware.bin` if you built for the XR), which you can then sign.

### Signing built firmware

After you've built the firmware, you'll need to sign it with the key found in your Onewheel. To get this key, you'll either need to physically de-solder and dump the STM32 off the board, or use the prebuilt version of this tool through [rewheel.app](https://rewheel.app).

Once you've got your boards key, can sign the built firmware by running the following commands: (once again swap out `pint` for `xr` if needed)
```sh
python3 pad.py .pio/build/pint/firmware.bin # Pad out firmware to make its length divisible by 32 bytes (for encryption)
openssl enc -aes-128-ecb -nosalt -nopad -K ENCRYPTION_KEY_HERE -in .pio/build/pint/firmware.bin -out pint-signed-extractor.bin
```
> **Note**: Don't forget to fill in your encryption key

This will create a file called `pint-signed-extractor.bin` that you can OTA upload to your board with a tool like [rewheel](https://rewheel.app).

