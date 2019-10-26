# sonoff-esp-homekit

This is my take on [Sonoff](https://www.amazon.de/gp/product/B07K5VS5GV) powered ceiling lights. Following projects inspired
me, so be sure to check them out as well:

- https://github.com/Gruppio/Sonoff-Homekit
- https://github.com/maximkulkin/esp-homekit-demo
- https://github.com/maximkulkin/esp-wifi-config

## Setup

Ensure Python is installed:

```sh
asdf install
```

```sh
pip install esptool pyserial
git submodule update --init --recursive
asdf reshim python
````

The Docker build will take a while (~30 min), so grab a coffee:

```sh
docker-compose build
```

## Usage

Update the Wi-Fi config in `esp-homekit-demo/wifi.h`:

```c
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"
```

The proper `tty.usbserial` can be found by running `ls /dev/tty.usb*`.

```sh
cd esp-homekit-demo

export SDK_PATH=`realpath ../esp-open-rtos`
export ESPPORT=/dev/tty.usbserial-1420

docker-compose run --rm rtos make -C examples/led all
make -C examples/led flash monitor
```

## [License (MIT)](./LICENSE)
