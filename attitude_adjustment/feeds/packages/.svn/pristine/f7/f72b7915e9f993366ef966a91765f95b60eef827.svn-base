#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=leafnode
PKG_VERSION:=1.11.8
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/leafnode
PKG_MD5SUM:=a3edafeb854efaa3fbb0f7951d02160f

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS+=--sysconfdir=/etc/leafnode
CONFIGURE_VARS+=LIBS=-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

EXTRA_CFLAGS+= $(TARGET_CPPFLAGS)

EXTRA_LDFLAGS+=-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Package/leafnode/Default
  SUBMENU:=NNTP
  SECTION:=net
  CATEGORY:=Network
  URL:=http://leafnode.sourceforge.net
endef

define Package/leafnode
  $(call Package/leafnode/Default)
  DEPENDS:=+libpcre +xinetd
  TITLE:=Leafnode store-and-forward NNTP proxy
endef

define Package/leafnode/description
	This package contains the "leafnode", "fetchnews" and
	"texpire" programs and the necessary configuration files
	for the leafnode proxy
endef

define Package/leafnode-extra
  $(call Package/leafnode/Default)
  DEPENDS:=leafnode
  TITLE:=Leafnode extra programs
endef

define Package/leafnode-extra/description
	This package contains the more rarely used "checkgroups",
	"applyfilter" and "leafnode-version" programs from the
	leafnode suite
endef

define Package/leafnode/conffiles
/etc/leafnode/config.example
/etc/xinetd.d/nntp
/etc/crontabs/news
endef

define Package/leafnode/install
  	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/leafnode $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/fetchnews $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/texpire $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/leafnode
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/leafnode/config.example $(1)/etc/leafnode/
	$(INSTALL_DIR) $(1)/etc/xinetd.d
	$(INSTALL_DATA) ./files/nntp $(1)/etc/xinetd.d/
	$(INSTALL_DIR) $(1)/etc/crontabs
	$(INSTALL_DATA) ./files/news $(1)/etc/crontabs/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/leafnode.init $(1)/etc/init.d/leafnode
endef

define Package/leafnode-extra/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/checkgroups $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/applyfilter $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/leafnode-version $(1)/usr/bin/
endef

$(eval $(call BuildPackage,leafnode))
$(eval $(call BuildPackage,leafnode-extra))
