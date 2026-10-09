/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: section define. \n
 *
 */

#ifndef LTO_ADAPT_H
#define LTO_ADAPT_H

#ifdef ENABLE_LTO_OPTIMIZE
#include "section_macros.h"

SECTION_FILE_PRAGMA_TEXT(LTO_COMPONENT_NAME)
SECTION_FILE_PRAGMA_RODATA(LTO_COMPONENT_NAME)
SECTION_FILE_PRAGMA_DATA(LTO_COMPONENT_NAME)
SECTION_FILE_PRAGMA_BSS(LTO_COMPONENT_NAME)
#endif

#endif