#*************************************************************************
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../../Makefile.param
    include $(PARAM_FILE)
endif

ifeq ($(CBB_PARAM_FILE), )
    CBB_PARAM_FILE:=../Makefile.param
    include $(CBB_PARAM_FILE)
endif

# kbuild 以 "make -C $(KERNEL_ROOT) ... src=$(CURDIR)" 二次读取本文件时，PWD 已被
# make 改写为内核目录，-I 与 include 全部落到错误位置（与 otp/klad、gfbg 同一类坑）。
# 改用 kbuild 传入的 src 定位自身目录，普通递归 make（未定义 src）时回退 $(PWD)。
BASED_BASE_DIR := $(if $(src),$(src),$(PWD))
#*************************************************************************
# local header include
MKP_CFLAGS+=-I $(BASED_BASE_DIR)/mkp/include
MKP_CFLAGS+=-I $(BASED_BASE_DIR)/arch/include
MKP_CFLAGS+=-I $(BASED_BASE_DIR)/arch/$(OT_ARCH)/include
MKP_CFLAGS+=-I $(BASED_BASE_DIR)/arch/$(OT_ARCH)/include/$(OT_SUBARCH)
MKP_CFLAGS+=-I $(AUTO_CONF_INC)

#*************************************************************************
# all source file in this module
SRCS:=

# src relative path
CBB_BASE_ROOT=./
CBB_BASE_ARCH=arch/$(OT_ARCH)/

# Makefile absolute path
include $(CBB_ROOT)/based/mkp/src/Makefile
include $(CBB_ROOT)/based/arch/$(OT_ARCH)/hal/Makefile
INIT_FILE := ./init/$(OSTYPE)/src/base_init.c
INIT_INC  := ./init/$(OSTYPE)/include/ot_base_mod_init.h
MKP_CFLAGS += -I$(BASED_BASE_DIR)/./init/$(OSTYPE)/include
SRCS_O := $(SRCS)
SRCS   += $(INIT_FILE)

#*************************************************************************
# release header
ifeq ($(CONFIG_BOTH_STYLE), y)
ADAPT_INC_FILE  := $(wildcard include/adapt/*.h)
ADAPT_INC_FILE  += $(wildcard $(CBB_ROOT)/include/adapt/*.h)
endif

MKP_CFLAGS :=$(subst base/,based/,$(MKP_CFLAGS))
MKP_CFLAGS :=$(subst sys/,sysd/,$(MKP_CFLAGS))

ifneq ($(CONFIG_OT_CROSS), llvm-)
MKP_CFLAGS += -Wno-address
endif

MOD_NAME := mod_base

#*************************************************************************
TARGET := $(KO_PREFIX)_base

#*************************************************************************
# compile linux or liteos
include $(BASED_BASE_DIR)/../Makefile.$(OSTYPE)
