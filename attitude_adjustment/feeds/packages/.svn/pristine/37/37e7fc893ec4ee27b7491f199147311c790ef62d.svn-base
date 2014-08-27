#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=heyu
PKG_VERSION:=2.9.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://heyu.tanj.com/download/
PKG_MD5SUM:=89d20d7f45109e7add5f0a141eac04eb

include $(INCLUDE_DIR)/package.mk

define Package/heyu
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+setserial
  TITLE:=X10 home automation control using a CM11A
  URL:=http://heyu.tanj.com/heyu2/
endef

define Package/heyu/description
	HEYU is a text-based console program for remotely controlling lights
	and appliances in the home or office.   It is made available under a
	free and open source license.

	Heyu uses the CM11A computer interface to send and receive X10 control
	signals over the AC power lines to modules which can turn On, Off, or
	Dim attached lamps or appliances.   It can store a schedule of timed
	events in the CM11A memory for execution when the computer is turned
	off or disconnected.
endef

define Package/heyu/conffiles
/etc/heyu/x10.conf
endef

define Build/Compile
	$(call Build/Compile/Default, \
		CFLAGS="$(TARGET_CFLAGS) \$$$$(DFLAGS)" \
	)
endef

define Package/heyu/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/heyu $(1)/usr/bin/

	$(INSTALL_DIR) $(1)/etc/$(PKG_NAME)
	$(INSTALL_DATA) ./files/x10.conf $(1)/etc/$(PKG_NAME)/x10.conf
endef

$(eval $(call BuildPackage,heyu))
