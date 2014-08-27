#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=gpiotoggling
PKG_RELEASE:=1
PKG_REV:=b57866441932eabc75b6c4252103121f606dc820
PKG_VERSION:=$(PKG_REV)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://github.com/mirko/gpio-toggling---linux-kernel-module.git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=git

include $(INCLUDE_DIR)/package.mk

define KernelPackage/gpiotoggling
  SUBMENU:=Other modules
  TITLE:=Toggle GPIOs in kernel space
  FILES:=$(PKG_BUILD_DIR)/gpiotoggling.ko
endef

define Build/Compile
	$(MAKE) -C $(LINUX_DIR) \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		modules
endef

$(eval $(call KernelPackage,gpiotoggling))
