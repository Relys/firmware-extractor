# Onewheel Firmware Extractor

A custom version of firmware designed to extract important information and keys from your Onewheel.

## Getting started

TODO, probably will link to rewheel.app.

## Build instructions

### Prerequesites

You'll need to have the latest version of the `arm-none-eabi` tools installed (to build everything) as well as `openssl` (to sign firmware). You can install them with:
```sh
sudo apt-get update
sudo apt-get install binutils-arm-none-eabi gcc-arm-none-eabi openssl
```

### Clone and build

To get the project going you'll need to clone and build the latest version of the repo. Once you've cloned it, building the binary is as simple as running `make`.

```sh
git clone git@github.com:exPHAT/ow-firmware-extractor.git
cd ow-firmware-extractor
make
```

That will create a compiled version of the firmare called `firmware.bin`, which you can then sign.

### Signing built firmware

After you've built the firmware, you'll need to sign it with the key found in your Onewheel. To get this key, you'll either need to physically de-solder and dump the STM32 off the board, or use the prebuilt version of this tool through [rewheel.app](https://rewheel.app).

Once you've got your boards key, can sign the built firmware by using:
```sh
make sign KEY=YOUR_EXTRACTED_KEY_HERE
```

This will create a file called `signed-firmware.bin` that you can OTA upload to your board with a tool like [rewheel](https://rewheel.app).

