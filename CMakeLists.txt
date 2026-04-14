# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(wifi_ble_scanner)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE src/main.c src/settings.c src/shell.c src/wifi.c)