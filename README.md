<!-- SPDX-License-Identifier: MIT -->

# wifi_ble_scanner

Minimal Zephyr application stub for the ESP32-C6 DevKit-C.

This application currently provides:

- a bootable Zephyr app target under `apps/wifi_ble_scanner`
- Wi-Fi support with Zephyr Wi-Fi shell commands over UART
- Wi-Fi credential storage through the `wifi_credentials` library
- app-local settings storage for boot auto-connect behavior
- log-based startup and Wi-Fi event messages using the Zephyr logging subsystem
- a verified build for `esp32c6_devkitc/esp32c6/hpcore`

It does not yet implement BLE scanning.

## Files

- `CMakeLists.txt`: Zephyr application entry point
- `include/wifi_ble_scanner/app_settings.h`: application settings interface
- `include/wifi_ble_scanner/app_shell.h`: application shell interface
- `include/wifi_ble_scanner/app_wifi.h`: application Wi-Fi interface
- `prj.conf`: application configuration
- `src/main.c`: startup orchestration
- `src/app_settings.c`: application-owned settings implementation
- `src/app_shell.c`: application shell commands
- `src/app_wifi.c`: Wi-Fi event handling and stored credential helpers
- `LICENSE`: local MIT license for this app subtree

## Verified Board Target

Use the HP core target for the ESP32-C6 DevKit-C:

```sh
esp32c6_devkitc/esp32c6/hpcore
```

## Build

From the workspace root:

```sh
.venv/bin/west build -p always \
  -b esp32c6_devkitc/esp32c6/hpcore \
  -s apps/wifi_ble_scanner \
  -d build/wifi_ble_scanner_esp32c6
```

## Flash

Flash the generated image:

```sh
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6
```

Flash to a specific UART port:

```sh
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6 --esp-device /dev/ttyUSB0
```

Alternative using an environment variable:

```sh
export ESPTOOL_PORT=/dev/ttyUSB0
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6
```

## Monitor

`west espressif monitor` must be run from inside the build directory. It does not accept a custom build directory argument.

```sh
cd build/wifi_ble_scanner_esp32c6
../../.venv/bin/west espressif monitor
```

Use an explicit serial port if needed:

```sh
cd build/wifi_ble_scanner_esp32c6
../../.venv/bin/west espressif -p /dev/ttyUSB0 monitor
```

Use an explicit baud rate if needed:

```sh
cd build/wifi_ble_scanner_esp32c6
../../.venv/bin/west espressif -p /dev/ttyUSB0 -b 115200 monitor
```

## Current Runtime Behavior

The application registers Wi-Fi management callbacks, enables UART shell access,
reports whether stored Wi-Fi credentials exist, and logs Wi-Fi connect and disconnect results.

Current implementation notes:

- logging only, no `printk`
- module name: `wifi_ble_scanner`
- log level: `INFO`
- UART shell commands are available through `wifi`, `wifi cred`, and `settings`
- app setting key: `wifi_ble_scanner/autoconnect_on_boot`

## UART Shell Usage

Build and flash the app, then open the monitor from the build directory.

Useful shell commands:

- `app reboot`
- `wifi scan`
- `wifi status`
- `wifi cred list`
- `wifi cred auto_connect`
- `wifi cred add -s "SSID" -k 1 -p "passphrase"`
- `settings read string wifi_ble_scanner/autoconnect_on_boot`
- `settings write string wifi_ble_scanner/autoconnect_on_boot true`
- `settings write string wifi_ble_scanner/autoconnect_on_boot false`

For open networks, omit the passphrase and use `-k 0`.

If `wifi_ble_scanner/autoconnect_on_boot` is set to `true`, the app requests `wifi cred auto_connect` during boot.

## Must Follow When Editing Code

These rules are mandatory for changes in this app subtree.

- Use Zephyr logging only. Do not introduce `printk`.
- Document public code interfaces in header files using Doxygen-style comments.
- Format C and header files according to the local `.clang-format` file.
- Use Google clang-format style with a maximum line length of 120 characters.
- Keep the target board as `esp32c6_devkitc/esp32c6/hpcore` unless there is an explicit reason to expand support.
- Preserve the local MIT licensing model. New source or config files in this subtree must use MIT SPDX headers where appropriate.
- Prefer small, isolated changes. Do not mix Wi-Fi work, BLE work, and unrelated cleanup in one edit.
- Keep modules small and focused. Split code into headers and source files once responsibilities stop being trivial.
- Prefer clear, explicit names for functions, types, variables, and macros. Avoid abbreviations unless they match Zephyr or protocol terminology.
- Keep public interfaces minimal. Make internal helpers `static` and expose only what other translation units need.
- Check and handle return values from Zephyr APIs and driver calls. Log failures with enough context to debug them.
- Use standard Zephyr error conventions. Return negative `errno` style values from functions that can fail.
- Avoid blocking work in callbacks or time-sensitive paths. Move longer operations into threads, work queues, or other appropriate Zephyr contexts.
- Keep concurrency explicit. Protect shared state correctly and document ownership of mutable data.
- Avoid global mutable state unless it is clearly justified and scoped to a well-defined module responsibility.
- Avoid magic numbers. Use named constants, enums, macros, or Kconfig options where the value has meaning.
- Prefer `const` correctness and narrow variable scope as much as possible.
- Validate inputs and state transitions early. Fail fast on invalid state rather than continuing with partial assumptions.
- Keep control flow simple. Avoid deeply nested conditionals when early returns or helper functions make intent clearer.
- Do not duplicate logic. Extract common code into helpers when the behavior is truly shared.
- Keep board-specific or feature-specific behavior behind clear compile-time guards or configuration options.
- Use Kconfig for feature toggles and tunables instead of hardcoding behavior that may need to vary.
- Keep includes minimal and ordered consistently. Include only what the file actually uses.
- Add comments only where they explain intent, constraints, or non-obvious behavior. Do not comment obvious code.
- Keep logging useful and bounded. Log state changes, failures, and important events, but avoid noisy logs in tight loops.
- Prefer fixed-width integer types and Zephyr/kernel types where interface clarity or hardware interaction requires them.
- Preserve bootability and a clean warning-free build as features are added incrementally.
- Keep startup and diagnostic output log-based so it works with the documented monitor flow.
- A code update is only valid if it passes the required build and tests.
- After changing code or configuration, rebuild with the verified command in this document, run the relevant tests for the change, and ensure the code is linted and formatted according to the local rules.
- If flash or monitor instructions change, update this README in the same change.
- If new Kconfig options are required, keep them in `prj.conf` unless there is a clear reason to split them into overlays or board-specific config.
- Keep this app bootable at every step, even if scanning features are still incomplete.

## Expected Next Steps

Likely follow-up implementation work:

- initialize BLE scanning
- initialize Wi-Fi scanning
- define how scan results are reported over logs
- add board- or feature-specific Kconfig options as functionality grows

## License

This app subtree is licensed under MIT. See `LICENSE`.