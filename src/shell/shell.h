/* SPDX-License-Identifier: MIT */

/**
 * @file shell.h
 * @brief Application shell command registration.
 */

#ifndef WIFI_BLE_SCANNER_SHELL_H_
#define WIFI_BLE_SCANNER_SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Register application shell commands.
 *
 * @details Registration is performed through linker macros when this module is
 * linked into the application. This declaration exists to document the module
 * responsibility.
 */
void wifi_ble_scanner_shell_init(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_BLE_SCANNER_SHELL_H_ */