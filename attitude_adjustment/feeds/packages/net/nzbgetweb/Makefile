#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nzbgetweb
PKG_VERSION:=1.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=@SF/nzbget
PKG_MD5SUM:=78ccce66f9df07ee69250ec79819c37c

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)/nzbgetweb

include $(INCLUDE_DIR)/package.mk

define Package/nzbgetweb
  SUBMENU:=NNTP
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+nzbget +php5 +php5-cgi +php5-mod-session +php5-mod-gd +php5-mod-xml +uhttpd
  URL:=http://nzbget.sourceforge.net/
  TITLE:=Web interface for nzbget
  MAINTAINER:=Artur Wronowski <arteqw@gmail.com>
endef

define Package/nzbgetweb/description
Web interface for nzbget.
endef

define Package/nzbgetweb/postinst
#!/bin/sh	
uci batch <<EOF
set uhttpd.nzbgetweb=uhttpd
set uhttpd.nzbgetweb.listen_http=0.0.0.0:8008
set uhttpd.nzbgetweb.script_timeout=60
set uhttpd.nzbgetweb.network_timeout=30
set uhttpd.nzbgetweb.interpreter=.php=/usr/bin/php-cgi
set uhttpd.nzbgetweb.home=/www/nzbgetweb
set uhttpd.nzbgetweb.index_page=index.php
commit uhttpd
EOF
/etc/init.d/uhttpd restart
echo "Bind your browser to router IP with port 8008"
endef

define Build/Compile
endef

define Package/nzbgetweb/install
	$(INSTALL_DIR) $(1)/www/nzbgetweb
	$(CP) $(PKG_BUILD_DIR)/*.php $(1)/www/nzbgetweb/
	$(CP) $(PKG_BUILD_DIR)/*.css $(1)/www/nzbgetweb/
	$(CP) $(PKG_BUILD_DIR)/README $(1)/www/nzbgetweb/
	$(CP) $(PKG_BUILD_DIR)/ChangeLog $(1)/www/nzbgetweb/
	$(CP) $(PKG_BUILD_DIR)/images $(1)/www/nzbgetweb/
	$(INSTALL_DIR) $(1)/www/nzbgetweb/lib
	$(CP) ./files/lib $(1)/www/nzbgetweb/lib/
endef

$(eval $(call BuildPackage,nzbgetweb))
