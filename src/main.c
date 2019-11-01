/*
 * Example of using esp-homekit library to control a simple $5 Sonoff Basic
 * using HomeKit.
 *
 * In order to flash the sonoff basic you will have to have a 3,3v (logic level)
 * FTDI adapter.
 *
 * To flash this example connect 3,3v, TX, RX, GND in this order, beginning in
 * the (square) pin header next to the button. Next hold down the button and
 * connect the FTDI adapter to your computer. The sonoff is now in flash mode
 * and you can flash the custom firmware.
 *
 * WARNING: Do not connect the sonoff to AC while it's connected to the FTDI
 * adapter! This may fry your computer and sonoff.
 */

#include <FreeRTOS.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <espressif/esp_common.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_wifi.h>
#include <stdio.h>
#include <task.h>

#include <homekit/characteristics.h>
#include <homekit/homekit.h>

#include "button.h"
#include "wifi.h"

// The GPIO pin that is connected to the relay on the Sonoff Basic.
const int relay_gpio = 12;
// The GPIO pin that is connected to the LED on the Sonoff Basic.
const int led_gpio = 13;
// The GPIO pin that is connected to the button on the Sonoff Basic.
const int button_gpio = 0;

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on,
                        void *context);
void button_callback(uint8_t gpio, button_event_t event);

void relay_write(bool on) { gpio_write(relay_gpio, on ? 1 : 0); }

void led_write(bool on) { gpio_write(led_gpio, on ? 0 : 1); }

void led_blink(int times) {
  for (int i = 0; i < times; i++) {
    led_write(true);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    led_write(false);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

void switch_identify_task(void *_args) {
  // We identify the Sonoff by Flashing it's LED.
  led_blink(3);
}

void switch_identify(homekit_value_t _value) {
  printf("Switch identify\n");
  xTaskCreate(switch_identify_task, "Switch identify", 128, NULL, 2, NULL);
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Sonoff Switch");

homekit_characteristic_t switch_on = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback = HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback));

void switch_on_callback(homekit_characteristic_t *_ch, homekit_value_t on,
                        void *context) {
  relay_write(switch_on.value.bool_value);
}

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(
            .id = 1, .category = homekit_accessory_category_switch,
            .services =
                (homekit_service_t *[]){
                    HOMEKIT_SERVICE(
                        ACCESSORY_INFORMATION,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                &name,
                                HOMEKIT_CHARACTERISTIC(MANUFACTURER,
                                                       "Mario Uher"),
                                HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER,
                                                       "037A2BABF19D"),
                                HOMEKIT_CHARACTERISTIC(MODEL, "Basic"),
                                HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION,
                                                       "1.0.0"),
                                HOMEKIT_CHARACTERISTIC(IDENTIFY,
                                                       switch_identify),
                                NULL}),
                    HOMEKIT_SERVICE(
                        SWITCH, .primary = true,
                        .characteristics =
                            (homekit_characteristic_t *[]){
                                HOMEKIT_CHARACTERISTIC(
                                    NAME,
                                    "Sonoff Ceiling Lights"),
                                &switch_on, NULL}),
                    NULL}),
    NULL};

void reset_configuration_task() {
  led_blink(5);

  homekit_server_reset();
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  sdk_system_restart();
  vTaskDelete(NULL);
}

void reset_configuration() {
  xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, 2,
              NULL);
}

void gpio_init() {
  gpio_enable(led_gpio, GPIO_OUTPUT);
  led_write(true);
  gpio_enable(relay_gpio, GPIO_OUTPUT);
  relay_write(switch_on.value.bool_value);
}

void button_callback(uint8_t gpio, button_event_t event) {
  switch (event) {
  case button_event_single_press:
    switch_on.value.bool_value = !switch_on.value.bool_value;
    relay_write(switch_on.value.bool_value);
    homekit_characteristic_notify(&switch_on, switch_on.value);
    break;

  case button_event_long_press:
    reset_configuration();
    break;

  default:
    // noop
    break;
  }
}

void create_accessory_name() {
  uint8_t macaddr[6];
  sdk_wifi_get_macaddr(STATION_IF, macaddr);

  int name_len = snprintf(NULL, 0, "Sonoff Switch-%02X%02X%02X", macaddr[3],
                          macaddr[4], macaddr[5]);
  char *name_value = malloc(name_len + 1);
  snprintf(name_value, name_len + 1, "Sonoff Switch-%02X%02X%02X", macaddr[3],
           macaddr[4], macaddr[5]);

  name.value = HOMEKIT_STRING(name_value);
}

static void wifi_init() {
  struct sdk_station_config wifi_config = {
      .ssid = WIFI_SSID,
      .password = WIFI_PASSWORD,
  };

  sdk_wifi_set_opmode(STATION_MODE);
  sdk_wifi_station_set_config(&wifi_config);
  sdk_wifi_station_connect();
}

homekit_server_config_t config = {.accessories = accessories,
                                  .password = "111-11-111"};

void user_init(void) {
  uart_set_baud(0, 115200);

  // Flash the LED on start
  led_blink(1);

  wifi_init();
  homekit_server_init(&config);
  create_accessory_name();

  gpio_init();
  button_create(button_gpio, 0, 4000, button_callback);
}
