#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# This Makefile is a skeleton
#

include $(TOPDIR)/rules.mk

PKG_NAME:=contiki
PKG_VERSION:=2.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=http://downloads.sourceforge.net/project/contiki/Contiki/Contiki%20$(PKG_VERSION)/
PKG_MD5SUM:=e2dbc5a42a899177d20d285c13bb3702

#PKG_BUILD_DIR:=$(BUILD_DIR)/tools
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/tunslip6
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-slip
  TITLE:=tunslip6 from Contiki OS
  URL:=http://www.contiki-os.org/
endef

define Package/tun6slip/description
  SLIP utility tool for connecting to Contiki 6LoWPAN devices.
endef

define Build/Compile
	# pass CFLAGS again to override -O2 set by configure
	$(MAKE) -C $(PKG_BUILD_DIR)/tools \
		CROSS_COMPILE="$(KERNEL_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		KERNELDIR=$(LINUX_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		tunslip6
endef

define Package/tunslip6/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/tools/tunslip6 $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,tunslip6))
