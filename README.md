# esp-homekit example

Ensure to check the following projects too:

- https://github.com/Gruppio/Sonoff-Homekit
- https://github.com/maximkulkin/esp-homekit-demo
- https://github.com/maximkulkin/esp-wifi-config

## Installation

```
pip install esptool pyserial
git submodule update --init --recursive
docker-compose build
```

## Usage

```
docker-compose build

export SDK_PATH=`pwd`/esp-open-rtos
export ESPPORT=/dev/tty.usbserial-1420

docker-compose run --rm rtos make -C examples/led all
make -C examples/led flash monitor
```
