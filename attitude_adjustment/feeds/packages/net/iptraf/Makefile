#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=iptraf
PKG_VERSION:=3.0.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://iptraf.seul.org/pub/iptraf/ \
	ftp://the.wiretapped.net/pub/security/network-monitoring/iptraf/
PKG_MD5SUM:=004c2c005a1b78739e22bc49d33e244d

include $(INCLUDE_DIR)/package.mk

define Package/iptraf
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses
  TITLE:=A console-based network monitoring program
  URL:=http://iptraf.seul.org/
endef

define Package/iptraf/description
  IPTraf is a console-based network statistics utility for Linux. It gathers a
  variety of figures such as TCP connection packet and byte counts, interface
  statistics and activity indicators, TCP/UDP traffic breakdowns, and LAN
  station packet and byte counts.
endef

define Package/iptraf/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	mkdir -p $${IPKG_INSTROOT}/var/log/iptraf \
		 $${IPKG_INSTROOT}/var/run/iptraf \
		 $${IPKG_INSTROOT}/var/lib/iptraf
fi
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		DEBUG="" \
		INCLUDEDIR="-I../support $(TARGET_CPPFLAGS)" \
		LDOPTS="$(TARGET_LDFLAGS)" \
		PLATFORM="-DPLATFORM=\\\"Linux/$(ARCH)\\\"" \
		TARGET="/usr/bin" \
		WORKDIR="/var/lib/iptraf" \
		all
endef

define Package/iptraf/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/iptraf $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/rvnamed $(1)/usr/bin/
endef

$(eval $(call BuildPackage,iptraf))
