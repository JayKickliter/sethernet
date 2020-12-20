# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

# Remember to clone zephr and set this environment variable before
# configuring the project.
#
# ZEPHYR_BASE=path/to/zephyrproject/zephyr

# For the time being, I do not care about using this project on
# different hardware. Let's keep it hard-coded (and preferably
# `west`-free) for now.
set(BOARD frdm_k64f)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CONF_FILE sethernet_defconfig)

# Default to GNU Arm Embedded toolchain if no toolchain is set
if(NOT ENV{ZEPHYR_TOOLCHAIN_VARIANT})
  set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
  find_program(GNU_ARM_GCC arm-none-eabi-gcc)
  if(NOT ${GNU_ARM_GCC} STREQUAL GNU_ARM_GCC-NOTFOUND)
    # The toolchain root is located above the path to the compiler.
    get_filename_component(GNUARMEMB_TOOLCHAIN_PATH ${GNU_ARM_GCC}/../.. ABSOLUTE)
  endif()
endif()


include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

project(sethernet)

target_sources(app PRIVATE src/main.c)
