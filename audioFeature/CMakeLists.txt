# Author : Weipeng He <heweipeng@gmail.com>
# Copyright (c) 2014, All rights reserved.

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

project(AudioFeature)

find_package(GSL REQUIRED)
include_directories(${GSL_INCLUDE_DIRS})

find_package(ALSA REQUIRED)
include_directories(${ALSA_INCLUDE_DIRS})

add_subdirectory(src)

