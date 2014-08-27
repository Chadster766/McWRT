#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=psplash
PKG_VERSION:=0.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://projects.o-hand.com/sources/psplash/
PKG_MD5SUM:=7649b7e0fac573eca2048a2a267e242b

PKG_INSTALL:=1

IMAGE_FILE:=generic
ifeq ($(CONFIG_TARGET_s3c24xx),y)
  IMAGE_FILE:=480x640
endif

include $(INCLUDE_DIR)/package.mk

define Package/psplash
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A boot splash screen
  URL:=http://projects.o-hand.com/psplash
endef

define Package/psplash/description
PSplash is a userspace graphical boot splash screen.
endef

define Build/Configure
	$(CP) \
		./files/image_$(IMAGE_FILE).h \
		$(PKG_BUILD_DIR)/psplash-owrt-img.h
	$(call Build/Configure/Default)
endef

define Package/psplash/install
	$(INSTALL_DIR) $(1)/bin $(1)/etc/init.d
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/bin/psplash* \
		$(1)/bin/
	$(INSTALL_BIN) \
		./files/psplash.init \
		$(1)/etc/init.d/psplash
endef

$(eval $(call BuildPackage,psplash))
