# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)

target_sources(app PRIVATE src/main.c
                           src/HalGpio.c
                           src/HalPwm.c
                           src/HalTim.c
                           src/SrvScheduler.c
                           src/UtilGen.c
                           src/AppEncoder.c
                           src/App.c)
