#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=asterisk18-chan-sccp-b
PKG_REV=3004
PKG_VERSION:=r$(PKG_REV)
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://svn.code.sf.net/p/chan-sccp-b/code/trunk
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=svn

include $(INCLUDE_DIR)/package.mk

define Package/asterisk18-chan-sccp-b
  SUBMENU:=Telephony
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SCCP channel provider for asterisk
  URL:=http://chan-sccp-b.sourceforge.net/
  DEPENDS:= asterisk18 +libltdl
endef

define Package/asterisk18-chan-sccp-b/description
 SCCP channel provider for asterisk. It delivers extended functionality for SCCP phones over chan_skinny delivered
 by asterisk by default.
endef

CONFIGURE_ARGS += \
	--with-asterisk=$(STAGING_DIR)/usr/include/asterisk-1.8

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CFLAGS="$(CFLAGS) -DLOW_MEMORY" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/asterisk18-chan-sccp-b/conffiles
/etc/asterisk/sccp.conf
endef

define Package/asterisk18-chan-sccp-b/install
	$(INSTALL_DIR) $(1)/etc/asterisk
	$(INSTALL_BIN)  ./files/sccp.openwrt.conf $(1)/etc/asterisk/sccp.conf
	$(INSTALL_DIR) $(1)/usr/lib/asterisk/modules
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/asterisk/modules/chan_sccp.so $(1)/usr/lib/asterisk/modules/
endef

$(eval $(call BuildPackage,asterisk18-chan-sccp-b))
