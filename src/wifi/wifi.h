/* SPDX-License-Identifier: MIT */

/**
 * @file wifi.h
 * @brief Wi-Fi lifecycle helpers for the application.
 */

#ifndef WIFI_BLE_SCANNER_WIFI_H_
#define WIFI_BLE_SCANNER_WIFI_H_

struct net_if;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Wi-Fi event handling for the application.
 *
 * @param[in] wifi_iface Wi-Fi interface used by the application.
 */
void wifi_ble_scanner_wifi_init(struct net_if* wifi_iface);

/**
 * @brief Log the number of stored Wi-Fi credentials.
 */
void wifi_ble_scanner_wifi_log_stored_credentials(void);

/**
 * @brief Request connection to stored Wi-Fi credentials.
 *
 * @param[in] wifi_iface Wi-Fi interface used by the application.
 *
 * @return 0 if the connection request was submitted, otherwise a negative
 * error code.
 */
int wifi_ble_scanner_wifi_request_autoconnect(struct net_if* wifi_iface);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_BLE_SCANNER_WIFI_H_ */