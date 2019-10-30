# sonoff-esp-homekit

This is my take on [Sonoff](https://www.amazon.de/gp/product/B07K5VS5GV) powered
ceiling lights. Following projects inspired me, so be sure to check them out as
well:

- https://github.com/Gruppio/Sonoff-Homekit
- https://github.com/maximkulkin/esp-homekit-demo
- https://github.com/maximkulkin/esp-homekit-demo/wiki/Build-instructions-ESP8266-(Docker)

## Setup

Ensure Python is installed:

```sh
asdf install
```

```sh
pip install esptool pyserial
asdf reshim python

git submodule update --init --recursive
````

The Docker build will take a while (~30 min), so grab a coffee:

```sh
docker-compose build
```

## Usage

Update the Wi-Fi config in `src/wifi.h`:

```c
#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "password"
```

The proper device can be found running `ls /dev/*usb*`. For an ESP connected via
an UART use the `/dev/cu.usbserial-*` device, for USB use `/dev/tty.usbserial-*`.

```sh
export SDK_PATH=`realpath ./esp-open-rtos`
export ESPPORT=/dev/cu.usbserial-1420

docker-compose run --rm rtos make -C src all
make -C src flash
```

If there are any problems updating the firmware try resetting the ESP:

```sh
make -C src erase_flash
```

## [License (MIT)](./LICENSE)
