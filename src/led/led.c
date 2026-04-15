/* SPDX-License-Identifier: MIT */

#include "led.h"

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led_strip.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(wifi_ble_scanner_led, LOG_LEVEL_INF);

#define STRIP_NODE DT_ALIAS(led_strip)

#if !DT_NODE_EXISTS(STRIP_NODE)
#error "wifi_ble_scanner requires a led-strip devicetree alias"
#endif

#if !DT_NODE_HAS_PROP(STRIP_NODE, chain_length)
#error "wifi_ble_scanner led-strip alias must define chain-length"
#endif

#define STRIP_NUM_PIXELS DT_PROP(STRIP_NODE, chain_length)

BUILD_ASSERT(STRIP_NUM_PIXELS > 0, "wifi_ble_scanner led-strip chain-length must be greater than zero");

static struct led_rgb pixels[STRIP_NUM_PIXELS];
static const struct device* const strip = DEVICE_DT_GET(STRIP_NODE);

static int wifi_ble_scanner_led_update(uint8_t red, uint8_t green, uint8_t blue) {
  int ret;

  if (!device_is_ready(strip)) {
    LOG_ERR("LED strip device %s is not ready", strip->name);
    return -ENODEV;
  }

  memset(pixels, 0x00, sizeof(pixels));
  pixels[0].r = red;
  pixels[0].g = green;
  pixels[0].b = blue;

  ret = led_strip_update_rgb(strip, pixels, STRIP_NUM_PIXELS);
  if (ret != 0) {
    LOG_ERR("led_strip_update_rgb failed: %d", ret);
  }

  return ret;
}

int wifi_ble_scanner_led_init(void) {
  int ret = wifi_ble_scanner_led_off();
  if (ret != 0) {
    return ret;
  }

  LOG_INF("Onboard RGB LED ready on %s", strip->name);
  return 0;
}

int wifi_ble_scanner_led_set_rgb(uint8_t red, uint8_t green, uint8_t blue) {
  return wifi_ble_scanner_led_update(red, green, blue);
}

int wifi_ble_scanner_led_off(void) { return wifi_ble_scanner_led_update(0, 0, 0); }