<!-- SPDX-License-Identifier: MIT -->

# wifi_ble_scanner

Minimal Zephyr application stub for the ESP32-C6 DevKit-C.

This application currently provides:

- a bootable Zephyr app target under `apps/wifi_ble_scanner`
- sysbuild support that builds MCUboot together with the application
- Wi-Fi support with Zephyr Wi-Fi shell commands over UART
- onboard WS2812 RGB LED control through app shell commands
- Wi-Fi credential storage through the `wifi_credentials` library
- app-local settings storage for boot auto-connect behavior
- MCUmgr SMP over UDP for remote management and image upload
- mDNS hostname advertisement derived from the Wi-Fi MAC address
- log-based startup and Wi-Fi event messages using the Zephyr logging subsystem
- a verified build for `esp32c6_devkitc/esp32c6/hpcore`

It does not yet implement BLE scanning.

## Verified Board Target

Use the HP core target for the ESP32-C6 DevKit-C:

```sh
esp32c6_devkitc/esp32c6/hpcore
```

## Build

For MCUboot-enabled firmware update support, build with sysbuild.

From the workspace root:

```sh
.venv/bin/west build --sysbuild -p always \
  -b esp32c6_devkitc/esp32c6/hpcore \
  -s apps/wifi_ble_scanner \
  -d build/wifi_ble_scanner_esp32c6_sysbuild
```

This builds both the application and MCUboot.

If you want an application-only build without MCUboot, omit `--sysbuild` and use a separate build directory.

## Flash

Flash the generated image:

```sh
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6_sysbuild
```

Flash to a specific UART port:

```sh
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6_sysbuild --esp-device /dev/ttyUSB0
```

Alternative using an environment variable:

```sh
export ESPTOOL_PORT=/dev/ttyUSB0
.venv/bin/west flash -d build/wifi_ble_scanner_esp32c6_sysbuild
```

## Monitor

`west espressif monitor` must be run from inside the build directory. It does not accept a custom build directory argument.

```sh
cd build/wifi_ble_scanner_esp32c6_sysbuild
../../.venv/bin/west espressif monitor
```

Use an explicit serial port if needed:

```sh
cd build/wifi_ble_scanner_esp32c6_sysbuild
../../.venv/bin/west espressif -p /dev/ttyUSB0 monitor
```

Use an explicit baud rate if needed:

```sh
cd build/wifi_ble_scanner_esp32c6_sysbuild
../../.venv/bin/west espressif -p /dev/ttyUSB0 -b 115200 monitor
```

## Current Runtime Behavior

The application registers Wi-Fi management callbacks, enables UART shell access,
reports whether stored Wi-Fi credentials exist, retries boot auto-connect until the first successful
connection, exposes MCUmgr over UDP, and logs Wi-Fi connect and disconnect results.

Current implementation notes:

- logging only, no `printk`
- module name: `wifi_ble_scanner`
- log level: `INFO`
- UART shell commands are available through `wifi`, `wifi cred`, and `settings`
- the device hostname is published through mDNS as `wifi-ble-scanner-<last-3-mac-octets>.local`
- app setting key: `wifi_ble_scanner/autoconnect_on_boot`
- MCUmgr over UDP supports image, OS, shell, settings, and stat management groups
- MCUmgr settings access is restricted to the `wifi_ble_scanner/*` subtree
- MCUboot is intended to be built through sysbuild

## MCUmgr Over UDP

The app enables MCUmgr SMP over UDP on IPv4.

Enabled MCUmgr groups:

- image management
- OS management
- shell management
- settings management
- statistics management

Remote settings access is limited to the application-owned `wifi_ble_scanner/*` subtree.

The device must have Wi-Fi connectivity and an IPv4 address before MCUmgr UDP requests can reach it.

The default UDP SMP port is the Zephyr MCUmgr transport default.

Typical workflow:

1. Build with `--sysbuild` so MCUboot and signed images are generated.
2. Flash the sysbuild output.
3. Connect the device to Wi-Fi and ensure it has an IPv4 address.
4. Use an MCUmgr-compatible client against the device IP over UDP for `image`, `os`, `shell`, and `settings` commands.

For image update, upload the signed application image produced by the sysbuild build.

With the verified build in this workspace, the signed application image is:

```sh
build/wifi_ble_scanner_esp32c6_sysbuild/wifi_ble_scanner/zephyr/zephyr.signed.bin
```

MCUboot itself is built in:

```sh
build/wifi_ble_scanner_esp32c6_sysbuild/mcuboot/zephyr/
```

## mDNS Hostname

The app enables the Zephyr mDNS responder and sets the device hostname at runtime from the last three octets of the Wi-Fi MAC address.

The hostname format is:

```sh
wifi-ble-scanner-<6 hex digits from the last 3 Wi-Fi MAC octets>
```

On the local network, the device is advertised as:

```sh
wifi-ble-scanner-<6 hex digits from the last 3 Wi-Fi MAC octets>.local
```

## UART Shell Usage

Build and flash the app, then open the monitor from the build directory.

Useful shell commands:

- `app name`
- `app led rgb 255 0 0`
- `app led off`
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
If the initial boot-time connection attempt fails or disconnects before the first successful connection, the app
retries automatically with capped exponential backoff.

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
- Keep module headers next to their implementation files. Do not reintroduce a separate top-level include tree for app-local modules.
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