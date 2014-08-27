#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=psybnc
PKG_VERSION:=2.3.2-7
PKG_RELEASE:=2

PKG_SOURCE:=psyBNC-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.psybnc.at/download/beta/
PKG_MD5SUM:=4b893a92f914813450e0921b2894ad6c

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/psybnc
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  TITLE:=An IRC-Bouncer with many features
  URL:=http://www.psybnc.at/
endef

define Package/psybnc/description
	psyBNC is an easy-to-use, multi-user, permanent 
	IRC-Bouncer with many features. Some of its features 
	vhost- and relay support to connected bouncers and an 
	extensive online help system. Many other helpful 
	functions are included.
endef

ifneq ($(SDK)$(CONFIG_BIG_ENDIAN),)
  ENDIANESS:=1
else
  ENDIANESS:=0
endif

define Build/Compile	
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS)" \
		HOSTCC="$(HOSTCC)" \
		all \
	)
endef

define Package/psybnc/conffiles
/etc/psybnc.conf
endef

define Package/psybnc/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/psybnc.sh $(1)/usr/sbin/psybnc
	$(INSTALL_DIR) $(1)/usr/share/psybnc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/psybnc $(1)/usr/share/psybnc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/psybnc.conf $(1)/usr/share/psybnc/
	$(INSTALL_DIR) $(1)/usr/share/psybnc/lang
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/lang/english.lng $(1)/usr/share/psybnc/lang/
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/psybnc.conf $(1)/etc
endef

$(eval $(call BuildPackage,psybnc))
