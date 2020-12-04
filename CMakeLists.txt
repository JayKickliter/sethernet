# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

set(BOARD frdm_k64f)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CONF_FILE sethernet_defconfig)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(sethernet)

target_sources(app PRIVATE src/main.c)
