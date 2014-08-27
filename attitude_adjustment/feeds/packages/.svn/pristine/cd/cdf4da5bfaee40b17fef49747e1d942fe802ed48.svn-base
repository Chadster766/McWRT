#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=oww
PKG_VERSION:=0.86.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/oww
PKG_MD5SUM:=8e51ffcd6008c9dc8a03a1a65f80092e

PKG_BUILD_DEPENDS:=intltool/host
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/oww
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=One-wire weather
  URL:=http://oww.sourceforge.net/
  DEPENDS:=+libcurl +libusb
endef

define Package/oww/description
  Oww is a software project, to communicate with the Dallas 1-wire weather
  station and other 1-wire sensors, presenting live weather readings, logging
  data to file, uploading data to weather Web sites, and sending data to other
  programs through a command line interface.
endef

CONFIGURE_ARGS += \
	--disable-gui

define Package/oww/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/oww.init $(1)/etc/init.d/oww
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/owwnogui $(1)/usr/bin/
endef

$(eval $(call BuildPackage,oww))
