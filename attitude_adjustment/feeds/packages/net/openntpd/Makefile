#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openntpd
PKG_VERSION:=3.9p1
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.openbsd.org/pub/OpenBSD/OpenNTPD/ \
	ftp://ftp.de.openbsd.org/pub/unix/OpenBSD/OpenNTPD/ \
	ftp://ftp.fr.openbsd.org/pub/OpenBSD/OpenNTPD/ \
	ftp://ftp.sunet.se/pub/OpenBSD/OpenNTPD/
PKG_MD5SUM:=afc34175f38d08867c1403d9008600b3

include $(INCLUDE_DIR)/package.mk

define Package/openntpd
  SUBMENU:=Time Synchronization
  SECTION:=net
  CATEGORY:=Network
  TITLE:=NTP server
  URL:=http://www.openntpd.org/
endef

define Package/openntpd/description
	A free and easy to use NTP (Network Time Protocol) implementation.
endef

define Package/openntpd/conffiles
/etc/ntpd.conf
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-builtin-arc4random \
		--with-privsep-user=network \
		--with-adjtimex \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) -DUSE_ADJTIMEX"
endef

define Package/openntpd/install	
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ntpd $(1)/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/ntpd.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ntpd.init $(1)/etc/init.d/ntpd
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/ntpd.hotplug $(1)/etc/hotplug.d/iface/20-ntpd
endef

define Package/openntpd/postinst
#!/bin/sh
[ -L "$${IPKG_INSTROOT}/usr/sbin/ntpd" ] && rm -f "$${IPKG_INSTROOT}/usr/sbin/ntpd"
exit 0
endef

define Package/openntpd/postrm
#!/bin/sh
/bin/busybox ntpd -h 2>&1 | grep -q BusyBox && ln -sf ../../bin/busybox /usr/sbin/ntpd
exit 0
endef

$(eval $(call BuildPackage,openntpd))
