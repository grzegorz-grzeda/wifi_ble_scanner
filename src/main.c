/* SPDX-License-Identifier: MIT */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(wifi_ble_scanner, LOG_LEVEL_INF);

int main(void) {
  LOG_INF("wifi_ble_scanner stub starting on %s", CONFIG_BOARD_TARGET);
  LOG_INF("Wi-Fi/BLE scanning is not implemented yet.");

  while (1) {
    k_msleep(1000);
  }

  return 0;
}