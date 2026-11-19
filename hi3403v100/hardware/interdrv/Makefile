ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../mpp/Makefile.param
	include $(PARAM_FILE)
endif

sub_dir:=

ifeq ($(CONFIG_MIPI_RX), y)
    sub_dir += mipi_rx
endif

ifeq ($(CONFIG_MIPI_TX), y)
    sub_dir += mipi_tx
endif

ifeq ($(CONFIG_OT_SYSCFG), y)
    sub_dir += sysconfig
endif

clean_dir:= $(foreach dir, $(sub_dir), $(patsubst %, %_clean, $(dir)))

.PHONY: all clean $(sub_dir) $(clean_dir)

all: $(sub_dir)

# cd 由 shell 更新 PWD（组件 Makefile 依赖 $(PWD)），&& 保证失败传播，$(MAKE) 递归
$(sub_dir):
	@cd $@ && $(MAKE)

clean: $(clean_dir)

$(clean_dir):
	@cd $(patsubst %_clean, %, $@) && $(MAKE) clean
