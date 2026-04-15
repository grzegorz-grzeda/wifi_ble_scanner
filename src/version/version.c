/* SPDX-License-Identifier: MIT */

#include "version.h"

#include <zephyr/app_version.h>
#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/version.h>

LOG_MODULE_DECLARE(wifi_ble_scanner, LOG_LEVEL_INF);

#define WIFI_BLE_SCANNER_BOOT_BANNER "------------------------------------------------"

#ifndef WIFI_BLE_SCANNER_GIT_COMMIT_HASH
#define WIFI_BLE_SCANNER_GIT_COMMIT_HASH "unknown"
#endif

#ifndef WIFI_BLE_SCANNER_GIT_DIRTY_SUFFIX
#define WIFI_BLE_SCANNER_GIT_DIRTY_SUFFIX ""
#endif

const char* wifi_ble_scanner_version_string(void) { return APP_VERSION_EXTENDED_STRING; }

const char* wifi_ble_scanner_zephyr_version_string(void) { return KERNEL_VERSION_STRING; }

const char* wifi_ble_scanner_git_commit_hash(void) { return WIFI_BLE_SCANNER_GIT_COMMIT_HASH; }

void wifi_ble_scanner_version_log_banner(void) {
  LOG_INF("%s", WIFI_BLE_SCANNER_BOOT_BANNER);
  LOG_INF("firmware: %s", wifi_ble_scanner_version_string());
  LOG_INF("git: %s%s", wifi_ble_scanner_git_commit_hash(), WIFI_BLE_SCANNER_GIT_DIRTY_SUFFIX);
  LOG_INF("zephyr: %s", wifi_ble_scanner_zephyr_version_string());
  LOG_INF("board: %s", CONFIG_BOARD_TARGET);
  LOG_INF("%s", WIFI_BLE_SCANNER_BOOT_BANNER);
}