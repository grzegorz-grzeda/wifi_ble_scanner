/* SPDX-License-Identifier: MIT */

/**
 * @defgroup wifi_ble_scanner_settings Settings Module
 * @ingroup wifi_ble_scanner_app
 * @brief Persistent application settings.
 */

/**
 * @file settings.h
 * @brief Application-owned settings interface.
 * @ingroup wifi_ble_scanner_settings
 */

#ifndef WIFI_BLE_SCANNER_SETTINGS_H_
#define WIFI_BLE_SCANNER_SETTINGS_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wifi_ble_scanner_settings
 * @{
 */

/**
 * @brief Settings subtree name for the application.
 */
#define WIFI_BLE_SCANNER_SETTINGS_NAME "wifi_ble_scanner"

/**
 * @brief Setting key that controls boot-time Wi-Fi auto-connect.
 */
#define WIFI_BLE_SCANNER_SETTING_AUTOCONNECT "autoconnect_on_boot"

/**
 * @brief Load application settings from persistent storage.
 *
 * @return 0 on success, otherwise a negative error code.
 */
int wifi_ble_scanner_settings_load(void);

/**
 * @brief Check whether Wi-Fi auto-connect is enabled for boot.
 *
 * @return true if enabled, otherwise false.
 */
bool wifi_ble_scanner_settings_autoconnect_on_boot(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* WIFI_BLE_SCANNER_SETTINGS_H_ */