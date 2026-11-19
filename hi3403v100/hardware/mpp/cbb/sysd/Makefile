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
SYSD_BASE_DIR := $(if $(src),$(src),$(PWD))
#*************************************************************************
# local header
MKP_CFLAGS+=-I $(SYSD_BASE_DIR)/mkp/include
MKP_CFLAGS+=-I $(SYSD_BASE_DIR)/arch/include
MKP_CFLAGS+=-I $(SYSD_BASE_DIR)/arch/$(OT_ARCH)/include
MKP_CFLAGS+=-I $(AUTO_CONF_INC)
ifeq ($(OT_ARCH), ss928v100)
MKP_CFLAGS+=-I $(SDK_PATH)/interdrv/$(OT_ARCH)/sysconfig/$(OT_SUBARCH)
else
MKP_CFLAGS+=-I $(SDK_PATH)/interdrv/$(OT_ARCH)/sysconfig
endif
MKP_CFLAGS :=$(subst base/,based/,$(MKP_CFLAGS))
MKP_CFLAGS :=$(subst sys/,sysd/,$(MKP_CFLAGS))
#*************************************************************************
# all source file in this module
#SRCS := mkp/src/sys_ext.c \
#        mkp/src/sys_bind.c
SRCS:=

# src relative path
CBB_SYS_ROOT=./
CBB_SYS_ARCH=arch/$(OT_ARCH)/

# Makefile absolute path
ifeq ($(CONFIG_DRIVER_SPACE_TYPE),user_space)
    include $(CBB_ROOT)/sysd/mkp/src/Makefile
    include $(CBB_ROOT)/sysd/arch/$(OT_ARCH)/hal/Makefile
else
    include $(CBB_ROOT)/sys/mkp/src/Makefile
    include $(CBB_ROOT)/sys/arch/$(OT_ARCH)/hal/Makefile
endif

INIT_FILE := ./init/$(OSTYPE)/src/sys_init.c
INIT_INC := ./init/$(OSTYPE)/include/ot_sys_mod_init.h
MKP_CFLAGS += -I$(SYSD_BASE_DIR)/./init/$(OSTYPE)/include
SRCS_O := $(SRCS)
SRCS += $(INIT_FILE)

#*************************************************************************
# release header

MOD_NAME := mod_sys

TARGET   := $(KO_PREFIX)_sys
#*************************************************************************
# compile linux or liteos
include $(SYSD_BASE_DIR)/../Makefile.$(OSTYPE)
