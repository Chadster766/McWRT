#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=marvellac

PKG_REV:=8.2.5.6.p3
PKG_VERSION:=v$(PKG_REV)
PKG_RELEASE:=fw-v7.2.5.6

#PKG_SOURCE_PROTO:=svn
#PKG_SOURCE_VERSION:=$(PKG_REV)
#PKG_SOURCE_SUBDIR:=$(if $(PKG_BRANCH),$(PKG_BRANCH),marvellac-trunk)-$(PKG_VERSION)
#PKG_SOURCE_URL:=http://marvellac-project.org/svn/marvellac/$(if $(PKG_BRANCH),branches/$(PKG_BRANCH),trunk)
#PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
#PKG_MIRROR_MD5SUM:=086b026d1c1561be8a949b79b0931404

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(if $(PKG_BRANCH),$(PKG_BRANCH),marvellac-trunk)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

MARVELLAC_FILES:=$(PKG_BUILD_DIR)/ap8x.ko
MARVELLAC_AUTOLOAD:=ap8x

define KernelPackage/marvellac
  SUBMENU:=Wireless Drivers
  TITLE:=Driver for Marvell wireless chipsets
  FILES:=$(MARVELLAC_FILES)
  AUTOLOAD:=$(call AutoLoad,10,$(MARVELLAC_AUTOLOAD))
  MENU:=1
endef

define KernelPackage/marvellac/description
 This package contains a driver for Marvell 802.11a/b/g chipsets.
endef

EXTRA_CFLAGS+= -I${LINUX_DIR}
EXTRA_CFLAGS+= -O2 -funroll-loops
EXTRA_CFLAGS+= -DMV_CPU_LE -DNUMOFAPS=8
EXTRA_CFLAGS+=-I${PKG_BUILD_DIR} -I${PKG_BUILD_DIR} -I${PKG_BUILD_DIR}/driver/linux -I${PKG_BUILD_DIR}/core/api -I${PKG_BUILD_DIR}/core/incl -I${PKG_BUILD_DIR}/core/mgt/STA/incl
#EXTRA_CFLAGS+=-I${SOC} -I${SOC}/Common -I${SOC}/Soc/idma -I${SOC}/Soc/ctrlEnv -I${SOC}/mv88f5181 -I${SOC}/osServices -I${SOC}/osServices/linux/ArchARM
#EXTRA_CFLAGS+=-I${SOC}/Board/boardEnv -I${SOC}/Board/boardEnv/DB_88FXX81 -I${SOC}/Soc/ctrlEnv/MV_88FXX81 -I${SOC}/osServices/linux

MAKE_ARGS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	TOOLPREFIX="$(KERNEL_CROSS)" \
	TOOLPATH="$(KERNEL_CROSS)" \
	KERNELPATH="$(LINUX_DIR)" \
	KDIR="$(LINUX_DIR)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)"

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./rel-drv-v8.2.5.6.p3-fw-v7.2.5.6/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
        $(MAKE) -C "$(LINUX_DIR)" $(MAKE_ARGS) modules
endef


#define Build/Compile
#	echo "test test"
#	$(MAKE_ARGS) $(MAKE) all
#endef

define KernelPackage/marvellac/install
	$(INSTALL_DIR) $(1)/usr/sbin
endef

$(eval $(call KernelPackage,marvellac))
