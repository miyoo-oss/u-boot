# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2004-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

# core
ifndef CONFIG_SPL_BUILD
obj-y += init/
obj-y += main.o
obj-y += exports.o
obj-$(CONFIG_HUSH_PARSER) += cli_hush.o
obj-$(CONFIG_AUTOBOOT) += autoboot.o

# # boards
obj-y += board_f.o
obj-y += board_r.o
obj-$(CONFIG_DISPLAY_BOARDINFO) += board_info.o
obj-$(CONFIG_DISPLAY_BOARDINFO_LATE) += board_info.o

obj-$(CONFIG_FDT_SIMPLEFB) += fdt_simplefb.o
obj-$(CONFIG_$(SPL_TPL_)OF_LIBFDT) += fdt_support.o
obj-$(CONFIG_MII) += miiphyutil.o
obj-$(CONFIG_CMD_MII) += miiphyutil.o
obj-$(CONFIG_PHYLIB) += miiphyutil.o

obj-$(CONFIG_USB_HOST) += usb.o usb_hub.o
obj-$(CONFIG_USB_GADGET) += usb.o usb_hub.o
obj-$(CONFIG_USB_STORAGE) += usb_storage.o

# others
obj-$(CONFIG_CONSOLE_MUX) += iomux.o
obj-$(CONFIG_MTD_NOR_FLASH) += flash.o
obj-$(CONFIG_CMD_KGDB) += kgdb.o kgdb_stubs.o
obj-$(CONFIG_I2C_EDID) += edid.o
obj-$(CONFIG_KALLSYMS) += kallsyms.o
obj-y += splash.o
obj-$(CONFIG_SPLASH_SOURCE) += splash_source.o
obj-$(CONFIG_MENU) += menu.o
obj-$(CONFIG_UPDATE_COMMON) += update.o
obj-$(CONFIG_USB_KEYBOARD) += usb_kbd.o
obj-$(CONFIG_CMDLINE) += cli_readline.o cli_simple.o

endif # !CONFIG_SPL_BUILD

obj-$(CONFIG_$(SPL_TPL_)BOOTSTAGE) += bootstage.o
obj-$(CONFIG_$(SPL_TPL_)BLOBLIST) += bloblist.o

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_SPL_DFU
obj-$(CONFIG_DFU_OVER_USB) += dfu.o
endif
obj-$(CONFIG_SPL_NET) += miiphyutil.o
obj-$(CONFIG_$(SPL_TPL_)OF_LIBFDT) += fdt_support.o

obj-$(CONFIG_SPL_USB_HOST) += usb.o usb_hub.o
obj-$(CONFIG_SPL_USB_STORAGE) += usb_storage.o
obj-$(CONFIG_SPL_MUSB_NEW) += usb.o
endif # CONFIG_SPL_BUILD

#others
obj-$(CONFIG_DDR_SPD) += ddr_spd.o
obj-$(CONFIG_SPD_EEPROM) += ddr_spd.o
obj-$(CONFIG_HWCONFIG) += hwconfig.o
obj-$(CONFIG_BOUNCE_BUFFER) += bouncebuf.o
ifdef CONFIG_SPL_BUILD
ifdef CONFIG_TPL_BUILD
obj-$(CONFIG_TPL_SERIAL) += console.o
else
obj-$(CONFIG_SPL_SERIAL) += console.o
endif
else
obj-y += console.o
endif # CONFIG_SPL_BUILD

obj-$(CONFIG_CROS_EC) += cros_ec.o
obj-y += dlmalloc.o
ifdef CONFIG_SYS_MALLOC_F
ifneq ($(CONFIG_$(SPL_TPL_)SYS_MALLOC_F_LEN),0)
obj-y += malloc_simple.o
endif
endif

obj-$(CONFIG_CYCLIC) += cyclic.o
obj-$(CONFIG_$(SPL_TPL_)EVENT) += event.o

obj-$(CONFIG_$(SPL_TPL_)HASH) += hash.o
obj-$(CONFIG_IO_TRACE) += iotrace.o
obj-y += memsize.o
obj-y += stdio.o

ifdef CONFIG_CMD_EEPROM_LAYOUT
obj-y += eeprom/eeprom_field.o eeprom/eeprom_layout.o
endif

obj-y += cli.o
obj-$(CONFIG_FSL_DDR_INTERACTIVE) += cli_simple.o cli_readline.o
obj-$(CONFIG_STM32MP1_DDR_INTERACTIVE) += cli_simple.o cli_readline.o
obj-$(CONFIG_DFU_OVER_USB) += dfu.o
obj-y += command.o
obj-$(CONFIG_$(SPL_TPL_)LOG) += log.o
obj-$(CONFIG_$(SPL_TPL_)LOG_CONSOLE) += log_console.o
obj-$(CONFIG_$(SPL_TPL_)LOG_SYSLOG) += log_syslog.o
obj-y += s_record.o
obj-$(CONFIG_CMD_LOADB) += xyzModem.o
obj-$(CONFIG_$(SPL_TPL_)YMODEM_SUPPORT) += xyzModem.o

obj-$(CONFIG_$(SPL_TPL_)AVB_VERIFY) += avb_verify.o
obj-$(CONFIG_$(SPL_TPL_)STACKPROTECTOR) += stackprot.o
obj-$(CONFIG_SCP03) += scp03.o

obj-$(CONFIG_QFW) += qfw.o


bj-y += cmd_auto_upgrade.o
obj-y += cmd_mstar.o cmd_osd.o
obj-$(CONFIG_MS_SDMMC) += cmd_sdmmc_ipv.o

ifdef CONFIG_CMD_CUSTOMER_INIT
obj-$(CONFIG_CMD_CUSTOMER_INIT) += cmd_customer_init.o
endif
ifdef CONFIG_MS_GPIO
obj-y += MsDebug.o
obj-$(CONFIG_MS_GPIO) += MsSysUtility.o
endif

ifdef CONFIG_MS_NAND
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/unfd/inc/api
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/unfd/inc/common
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/unfd/inc/config
endif

ifdef CONFIG_MS_SPINAND
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/spinand/inc/common
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/spinand/inc/config
CFLAGS_env_nand.o  += -I$(srctree)/drivers/mstar/spinand/inc/config/$(CONFIG_SYS_CONFIG_NAME)
CFLAGS_cmd_mstar.o  += -I$(srctree)/drivers/mstar/spinand/inc/config/$(CONFIG_SYS_CONFIG_NAME)
CFLAGS_cmd_nand.o  += -I$(srctree)/drivers/mstar/spinand/inc/config/$(CONFIG_SYS_CONFIG_NAME)
endif

ifdef CONFIG_MS_SRCFG
obj-y += cmd_srcfg.o
endif

CFLAGS_cmd_mstar.o += -I$(srctree)
CFLAGS_env_sf.o += -I$(srctree)


ifdef CONFIG_SSTAR_IR
CFLAGS_autoboot.o += -I$(srctree)/drivers/mstar/ir
endif

ifdef CONFIG_SSTAR_DISP
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/common
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/disp/drv/midisp/pub
endif

ifdef CONFIG_SSTAR_PNL
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/common
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/panel/drv/mipnl/pub
endif

ifdef CONFIG_SSTAR_HDMITX
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/common
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/hdmitx/drv/hdmitx/pub
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/hdmitx/drv/mihdmitx/pub
endif

ifdef CONFIG_SSTAR_JPD
CHIP_NAME := $(subst ",,$(CONFIG_SYS_CONFIG_NAME))
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/jpd/$(CHIP_NAME)/inc
endif

ifdef CONFIG_SSTAR_RGN
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/rgn/drv/pub
endif

ifdef CONFIG_MS_PARTITION
CFLAGS_cmd_bootlogo.o += -I$(srctree)/drivers/mstar/partition
CFLAGS_autoboot.o += -I$(srctree)/drivers/mstar/partition
endif
