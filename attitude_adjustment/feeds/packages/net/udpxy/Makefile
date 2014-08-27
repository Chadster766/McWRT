#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=udpxy
PKG_REV:=1.0.21
PKG_TAG:=2
PKG_VERSION:=$(PKG_REV)-$(PKG_TAG)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME).$(PKG_REV)-$(patsubst %,%-prod,$(PKG_TAG)).tgz
PKG_SOURCE_URL:=@SF/udpxy
PKG_MD5SUM:=d969fc34604b3abcc8ef62510967f2b0

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/udpxy
  SECTION:=net
  CATEGORY:=Network
  TITLE:=convert UDP IPTV streams into http stream
  URL:=http://sourceforge.net/projects/udpxy
endef

define Package/udpxy/description
	udproxy makes it possible to convert UDP IPTV streams into http
	streams which can be viewed even over wlans. http streams do
	not generate huge amounts of multicast traffic, so a sd stream
	only takes about 300k. interesting for peoply who have IPTV at
	home and do not want to rent multiple decoders from their
	provider but just use their own streaming client for example
	popcornhour/mediatomb/vlc/etc.
endef

MAKE_FLAGS += \
	INSTALLROOT="$(PKG_INSTALL_DIR)/usr"

define Package/udpxy/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/udpxy.init $(1)/etc/init.d/udpxy
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/udpxy $(1)/usr/bin/
	ln -sf udpxy $(1)/usr/bin/udpxrec
endef

$(eval $(call BuildPackage,udpxy))
