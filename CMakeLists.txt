# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(app1)

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/net/ip)

target_sources(app PRIVATE src/main.c)
