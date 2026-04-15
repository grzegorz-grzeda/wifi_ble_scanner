/* SPDX-License-Identifier: MIT */

#include "shell.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>

#include "led/led.h"
#include "wifi/wifi.h"

static int shell_parse_u8(const struct shell* shell, const char* value, const char* channel_name, uint8_t* parsed) {
  char* end = NULL;
  unsigned long converted;

  errno = 0;
  converted = strtoul(value, &end, 0);
  if ((errno != 0) || (end == value) || (*end != '\0') || (converted > UINT8_MAX)) {
    shell_error(shell, "%s must be an integer in the range 0-255.", channel_name);
    return -EINVAL;
  }

  *parsed = (uint8_t)converted;
  return 0;
}

static int shell_cmd_name(const struct shell* shell, size_t argc, char* argv[]) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "mDNS device name: %s.local", wifi_ble_scanner_wifi_get_device_name());
  return 0;
}

static int shell_cmd_reboot(const struct shell* shell, size_t argc, char* argv[]) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "Rebooting...");
  sys_reboot(SYS_REBOOT_COLD);

  return 0;
}

static int shell_cmd_led_rgb(const struct shell* shell, size_t argc, char* argv[]) {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  int ret;

  ARG_UNUSED(argc);

  ret = shell_parse_u8(shell, argv[1], "red", &red);
  if (ret != 0) {
    return ret;
  }

  ret = shell_parse_u8(shell, argv[2], "green", &green);
  if (ret != 0) {
    return ret;
  }

  ret = shell_parse_u8(shell, argv[3], "blue", &blue);
  if (ret != 0) {
    return ret;
  }

  ret = wifi_ble_scanner_led_set_rgb(red, green, blue);
  if (ret != 0) {
    shell_error(shell, "Failed to set onboard RGB LED: %d", ret);
    return ret;
  }

  shell_print(shell, "Onboard RGB LED set to R:%u G:%u B:%u", red, green, blue);
  return 0;
}

static int shell_cmd_led_off(const struct shell* shell, size_t argc, char* argv[]) {
  int ret;

  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  ret = wifi_ble_scanner_led_off();
  if (ret != 0) {
    shell_error(shell, "Failed to turn off onboard RGB LED: %d", ret);
    return ret;
  }

  shell_print(shell, "Onboard RGB LED turned off.");
  return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    shell_led_cmds, SHELL_CMD_ARG(rgb, NULL, "Set onboard RGB LED: rgb <red> <green> <blue>.", shell_cmd_led_rgb, 4, 0),
    SHELL_CMD_ARG(off, NULL, "Turn the onboard RGB LED off.", shell_cmd_led_off, 1, 0), SHELL_SUBCMD_SET_END);

SHELL_STATIC_SUBCMD_SET_CREATE(shell_cmds,
                               SHELL_CMD_ARG(name, NULL, "Print the current mDNS device name.", shell_cmd_name, 1, 0),
                               SHELL_CMD(led, &shell_led_cmds, "Control the onboard RGB LED.", NULL),
                               SHELL_CMD_ARG(reboot, NULL, "Reboot the device.", shell_cmd_reboot, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(app, &shell_cmds, "Application commands", NULL);

void wifi_ble_scanner_shell_init(void) {}