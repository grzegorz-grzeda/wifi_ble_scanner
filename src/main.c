/* SPDX-License-Identifier: MIT */

#include <wifi_ble_scanner/settings.h>
#include <wifi_ble_scanner/shell.h>
#include <wifi_ble_scanner/wifi.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>

LOG_MODULE_REGISTER(wifi_ble_scanner, LOG_LEVEL_INF);

static void main_maybe_autoconnect(struct net_if* wifi_iface) {
  int ret;

  if (!wifi_ble_scanner_settings_autoconnect_on_boot()) {
    LOG_INF("Boot auto-connect is disabled. Use settings write string %s/%s true to enable it.",
            WIFI_BLE_SCANNER_SETTINGS_NAME, WIFI_BLE_SCANNER_SETTING_AUTOCONNECT);
    return;
  }

  ret = wifi_ble_scanner_wifi_request_autoconnect(wifi_iface);
  if (ret != 0) {
    LOG_WRN("Stored Wi-Fi auto-connect request failed: %d", ret);
    return;
  }

  LOG_INF("Stored Wi-Fi auto-connect requested.");
}

int main(void) {
  struct net_if* wifi_iface = net_if_get_first_wifi();
  int ret;

  LOG_INF("wifi_ble_scanner starting on %s", CONFIG_BOARD_TARGET);

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

  LOG_INF("UART shell is ready. Use 'app', 'wifi', 'wifi cred', and 'settings' commands.");
  wifi_ble_scanner_wifi_log_stored_credentials();
  main_maybe_autoconnect(wifi_iface);
  LOG_INF("BLE scanning is not implemented yet.");

  return 0;
}