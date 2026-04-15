/* SPDX-License-Identifier: MIT */

/**
 * @defgroup wifi_ble_scanner_led LED Module
 * @ingroup wifi_ble_scanner_app
 * @brief Onboard RGB LED control.
 */

/**
 * @file led.h
 * @brief Onboard RGB LED control helpers.
 * @ingroup wifi_ble_scanner_led
 */

#ifndef WIFI_BLE_SCANNER_LED_H_
#define WIFI_BLE_SCANNER_LED_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup wifi_ble_scanner_led
 * @{
 */

/**
 * @brief Initialize the onboard RGB LED module.
 *
 * @return 0 on success, otherwise a negative error code.
 */
int wifi_ble_scanner_led_init(void);

/**
 * @brief Set the onboard RGB LED color.
 *
 * @param[in] red Red channel value.
 * @param[in] green Green channel value.
 * @param[in] blue Blue channel value.
 *
 * @return 0 on success, otherwise a negative error code.
 */
int wifi_ble_scanner_led_set_rgb(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Turn the onboard RGB LED off.
 *
 * @return 0 on success, otherwise a negative error code.
 */
int wifi_ble_scanner_led_off(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* WIFI_BLE_SCANNER_LED_H_ */