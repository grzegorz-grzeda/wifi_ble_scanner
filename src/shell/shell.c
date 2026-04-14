/* SPDX-License-Identifier: MIT */

#include "shell.h"

#include <zephyr/shell/shell.h>
#include <zephyr/sys/reboot.h>

static int shell_cmd_reboot(const struct shell* shell, size_t argc, char* argv[]) {
  ARG_UNUSED(argc);
  ARG_UNUSED(argv);

  shell_print(shell, "Rebooting...");
  sys_reboot(SYS_REBOOT_COLD);
}

SHELL_STATIC_SUBCMD_SET_CREATE(shell_cmds, SHELL_CMD_ARG(reboot, NULL, "Reboot the device.", shell_cmd_reboot, 1, 0),
                               SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(app, &shell_cmds, "Application commands", NULL);

void wifi_ble_scanner_shell_init(void) {}