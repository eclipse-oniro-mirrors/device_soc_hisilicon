#===============================================================================
# @brief    cmake file
# Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
#===============================================================================
set(COMPONENT_NAME "dma")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/dma.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "*.h")

build_component()
