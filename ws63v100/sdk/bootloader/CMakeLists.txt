#===============================================================================
# @brief    cmake file
# Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
#===============================================================================
add_subdirectory_if_exist(bootrom)
add_subdirectory_if_exist(flashboot)
add_subdirectory_if_exist(provision)

if ((${CHIP} STREQUAL "bs21") OR (${CHIP} STREQUAL "bs21a") OR (${CHIP} STREQUAL "ws63") OR (${CHIP} STREQUAL "sw39") OR (${CHIP} STREQUAL "sw21"))
add_subdirectory_if_exist(commonboot)
endif()

add_subdirectory_if_exist(provision_${CHIP})
add_subdirectory_if_exist(flashboot_${CHIP})
add_subdirectory_if_exist(bootrom_${CHIP})
