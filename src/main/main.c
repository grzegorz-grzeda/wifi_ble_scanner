/* SPDX-License-Identifier: MIT */

#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>

#include "led/led.h"
#include "settings/settings.h"
#include "shell/shell.h"
#include "wifi/wifi.h"

LOG_MODULE_REGISTER(wifi_ble_scanner, LOG_LEVEL_INF);

static void main_maybe_autoconnect(struct net_if* wifi_iface) {
  int ret;

  if (!wifi_ble_scanner_settings_autoconnect_on_boot()) {
    LOG_INF("Boot auto-connect is disabled. Use settings write string %s/%s true to enable it.",
            WIFI_BLE_SCANNER_SETTINGS_NAME, WIFI_BLE_SCANNER_SETTING_AUTOCONNECT);
    return;
  }

  ret = wifi_ble_scanner_wifi_start_autoconnect(wifi_iface);
  if (ret != 0) {
    LOG_WRN("Stored Wi-Fi auto-connect startup failed: %d", ret);
    return;
  }

  LOG_INF("Stored Wi-Fi auto-connect started.");
}

int main(void) {
  struct net_if* wifi_iface = net_if_get_first_wifi();
  int ret;

  LOG_INF("wifi_ble_scanner starting on %s", CONFIG_BOARD_TARGET);

  ret = wifi_ble_scanner_led_init();
  if (ret != 0) {
    LOG_WRN("Onboard RGB LED init failed: %d", ret);
  }

  if (wifi_iface == NULL) {
    LOG_ERR("No Wi-Fi interface found.");
    return 0;
  }

  wifi_ble_scanner_shell_init();
  wifi_ble_scanner_wifi_init(wifi_iface);

  ret = wifi_ble_scanner_settings_load();
  if (ret != 0) {
    LOG_WRN("settings_load failed: %d", ret);
  }

  wifi_ble_scanner_wifi_log_stored_credentials();
  main_maybe_autoconnect(wifi_iface);

  return 0;
}