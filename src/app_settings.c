/* SPDX-License-Identifier: MIT */

#include <stdbool.h>
#include <string.h>
#include <wifi_ble_scanner/app_settings.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

LOG_MODULE_DECLARE(wifi_ble_scanner, LOG_LEVEL_INF);

static bool autoconnect_on_boot;

static bool parse_bool_setting(const char* value, size_t len, bool* parsed_value) {
  if (len == 1U && value[0] == '0') {
    *parsed_value = false;
    return true;
  }

  if (len == 1U && value[0] == '1') {
    *parsed_value = true;
    return true;
  }

  if (len == 4U && strncmp(value, "true", len) == 0) {
    *parsed_value = true;
    return true;
  }

  if (len == 5U && strncmp(value, "false", len) == 0) {
    *parsed_value = false;
    return true;
  }

  return false;
}

static int app_settings_set(const char* name, size_t len, settings_read_cb read_cb, void* cb_arg) {
  char value[6] = {0};
  const char* next = NULL;
  ssize_t read_len;
  bool parsed_value;

  if (!settings_name_steq(name, WIFI_BLE_SCANNER_SETTING_AUTOCONNECT, &next) || next != NULL) {
    return -ENOENT;
  }

  if (len == 0U || len >= sizeof(value)) {
    return -EINVAL;
  }

  read_len = read_cb(cb_arg, value, len);
  if (read_len < 0) {
    return (int)read_len;
  }

  if ((size_t)read_len != len) {
    return -EINVAL;
  }

  if (!parse_bool_setting(value, len, &parsed_value)) {
    return -EINVAL;
  }

  autoconnect_on_boot = parsed_value;
  return 0;
}

static int app_settings_commit(void) {
  LOG_INF("Setting %s/%s=%s", WIFI_BLE_SCANNER_SETTINGS_NAME, WIFI_BLE_SCANNER_SETTING_AUTOCONNECT,
          autoconnect_on_boot ? "true" : "false");
  return 0;
}

static int app_settings_export(int (*cb)(const char* name, const void* value, size_t val_len)) {
  static const char enabled[] = "true";
  static const char disabled[] = "false";
  const char* value = autoconnect_on_boot ? enabled : disabled;

  return cb(WIFI_BLE_SCANNER_SETTINGS_NAME "/" WIFI_BLE_SCANNER_SETTING_AUTOCONNECT, value, strlen(value));
}

SETTINGS_STATIC_HANDLER_DEFINE(wifi_ble_scanner, WIFI_BLE_SCANNER_SETTINGS_NAME, NULL, app_settings_set,
                               app_settings_commit, app_settings_export);

int wifi_ble_scanner_settings_load(void) { return settings_load(); }

bool wifi_ble_scanner_settings_autoconnect_on_boot(void) { return autoconnect_on_boot; }