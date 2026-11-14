#*******************************************************************************
#The rule of make Linux kernel module
#*******************************************************************************
ifeq ($(PARAM_FILE), )
    PARAM_FILE:=../hardware/mpp/Makefile.param
    include $(PARAM_FILE)
endif

EXTRA_CFLAGS += $(MPP_CFLAGS)
EXTRA_CFLAGS += -I$(REL_INC) -I$(AUTO_CONF_INC)
EXTRA_CFLAGS += -I$(OSAL_ROOT)/include
EXTRA_CFLAGS += -I$(REL_DIR)/mpp/component/gfbg/drv/include

default:
	@echo -e "\e[0;32;1m--Compiling '$(TARGET)'... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS) REL_INC=$(REL_INC)
	@echo ---- OT_SUBARCH=$(OT_SUBARCH), CVER=$(CVER), DBG=$(OT_DBG), OT_FPGA=$(OT_FPGA)
	@echo ---- CPU_TYPE=$(CPU_TYPE) REL_KO=$(REL_KO)
	@echo ---- EXTRA_CFLAGS=$(EXTRA_CFLAGS)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- KERNEL_ROOT=$(KERNEL_ROOT)
	@echo ---- ARCH_ROOT=$(ARCH_ROOT), ARCH_HAL=$(ARCH_HAL)
	@echo ---- obj-m=$(obj-m)
	@@echo -e "\e[0m"
	@cd $(SDK_PATH)/mpp/component/securec && $(MAKE)
	@cd $(SDK_PATH)/osal/$(OSTYPE)/kernel && $(MAKE)
	@cd $(SDK_PATH)/interdrv && $(MAKE)
	@cd $(SDK_PATH)/interdrv/ot_irq && $(MAKE)
	@cd $(SDK_PATH)/interdrv/ot_proc && $(MAKE)
	@cd $(SDK_PATH)/mpp/component/gfbg && $(MAKE)
	@cd $(SDK_PATH)/mpp/component/security_subsys && $(MAKE)
	@cd $(SDK_PATH)/mpp/cbb/based && $(MAKE)
	@cd $(SDK_PATH)/mpp/cbb/sysd && $(MAKE)
	@cd $(SDK_PATH)/mpp/cbb/hdmi/src && $(MAKE)
	@cd $(SDK_PATH)/mpp/cbb/tde/tde/drv && $(MAKE)
	@cd $(SDK_PATH)/mpp/cbb/vo/vo_dev && $(MAKE)

clean:
	@rm -f *.ko
	@rm .tmp_versions  -rf
	@rm Module.symvers -f
	@rm modules.order  -f
	@rm .*.cmd  -f
	@rm *.mod.c  -f
	@rm *.mod.o  -f
	@rm $(KO_PREFIX)_*.o  -f
	@cd $(SDK_PATH)/mpp/component/securec && $(MAKE) clean
	@cd $(SDK_PATH)/interdrv && $(MAKE) clean
	@cd $(SDK_PATH)/interdrv/ot_irq && $(MAKE) clean
	@cd $(SDK_PATH)/interdrv/ot_proc && $(MAKE) clean
	@cd $(SDK_PATH)/osal/$(OSTYPE)/kernel && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/component/gfbg && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/component/security_subsys && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/cbb/based && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/cbb/sysd && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/cbb/hdmi/src && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/cbb/tde/tde/drv && $(MAKE) clean
	@cd $(SDK_PATH)/mpp/cbb/vo/vo_dev && $(MAKE) clean