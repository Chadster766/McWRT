# Copyright (C) 2008-2010, OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipmitool
PKG_VERSION:=1.8.11
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/ipmitool
PKG_MD5SUM:=1d0da20add7388d64c549f95538b6858

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/ipmitool
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libopenssl +libncurses +libreadline
  TITLE:=IPMI remote computer management utility
  URL:=http://ipmitool.sourceforge.net/
endef

define Package/ipmitool/description
  IPMItool is a utility for managing and configuring devices
  that support the Intelligent Platform Management Interface
  (IPMI) version 1.5 and version 2.0 specifications.
endef

CONFIGURE_ARGS += \
	--enable-static=no \
	--enable-internal-md5 \
	--enable-intf-lan=yes \
	--enable-intf-lanplus=no \
	--enable-intf-free=no \
	--enable-intf-open=no \
	--enable-intf-imb=no \
	--enable-intf-lipmi=no \
	--enable-intf-bmc=no

define Package/ipmitool/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ipmitool $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,ipmitool))

