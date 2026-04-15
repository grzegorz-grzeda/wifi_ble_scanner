/* SPDX-License-Identifier: MIT */

/**
 * @defgroup wifi_ble_scanner_version Version Module
 * @ingroup wifi_ble_scanner_app
 * @brief Application version and startup banner helpers.
 */

/**
 * @file version.h
 * @brief Application version and startup banner helpers.
 * @ingroup wifi_ble_scanner_version
 */

#ifndef WIFI_BLE_SCANNER_VERSION_H_
#define WIFI_BLE_SCANNER_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wifi_ble_scanner_version
 * @{
 */

/**
 * @brief Get the application version string.
 *
 * @return Pointer to the generated Zephyr application version string.
 */
const char* wifi_ble_scanner_version_string(void);

/**
 * @brief Get the Zephyr kernel version string.
 *
 * @return Pointer to the generated Zephyr kernel version string.
 */
const char* wifi_ble_scanner_zephyr_version_string(void);

/**
 * @brief Get the application git commit hash.
 *
 * @return Pointer to the build-time git commit hash, or "unknown".
 */
const char* wifi_ble_scanner_git_commit_hash(void);

/**
 * @brief Log the startup banner with version and board information.
 */
void wifi_ble_scanner_version_log_banner(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* WIFI_BLE_SCANNER_VERSION_H_ */