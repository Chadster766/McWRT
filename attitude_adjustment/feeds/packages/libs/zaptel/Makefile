# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=zaptel
PKG_VERSION:=1.2.27
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.asterisk.org/pub/telephony/zaptel/releases/
PKG_MD5SUM:=6e0ccf2a2a805054123b35bf8eb10ae9

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/zaptel-libtonezone
  SECTION:=libs
  CATEGORY:=Libraries
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Zaptel tonezone library.
  URL:=http://downloads.asterisk.org/pub/telephony/zaptel/
endef

define Build/Configure
endef

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		HOSTCC="$(HOSTCC)" \
		CFLAGS="$(TARGET_CFLAGS) -I. -DBUILDING_TONEZONE" \
		KSRC="$(LINUX_DIR)" \
		KVERS="$(LINUX_VERSION)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install-tonezone
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/linux
	$(CP) $(PKG_INSTALL_DIR)/usr/include/linux/zaptel.h $(1)/usr/include/linux/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/tonezone.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtonezone.so* $(1)/usr/lib/
endef

define Package/zaptel-libtonezone/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtonezone.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,zaptel-libtonezone))
