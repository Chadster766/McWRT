#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dir300-flash
PKG_VERSION:=0.9.9
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://x-alina.freifunk-potsdam.de/downloads/dir300-flash
PKG_MD5SUM:=84ad98c36393d7d238a229da67e23903

include $(INCLUDE_DIR)/package.mk

define Package/dir300-flash
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Flashes a custom firmware on the DIR-300 wireless router
  URL:=http://x-alina.freifunk-potsdam.de/
  PKGARCH:=all
endef

define Download/ap61.ram
  URL:=http://www.dd-wrt.com/dd-wrtv2/downloads/v24/Atheros%20WiSoc/Airlink%20101%20AR430W
  FILE:=ap61.ram
  MD5SUM:=4aaec2bff1edffe8c49003ed9363ad8b
endef

define Download/ap61.rom
  URL:=http://www.dd-wrt.com/dd-wrtv2/downloads/v24/Atheros%20WiSoc/Airlink%20101%20AR430W
  FILE:=ap61.rom
  MD5SUM:=eb7817d202c297ae5dc6b6b897632601
endef

define Download/dir300redboot.zip
  URL:=http://www.shadowandy.net/wp/wp-content/uploads
  FILE:=dir300redboot.zip
  MD5SUM:=5fdb24432aa200b508026215d821e126
endef

define Build/Compile
	unzip -n '$(DL_DIR)/dir300redboot.zip' -d '$(PKG_BUILD_DIR)'
endef

define Package/dir300-flash/install
	$(INSTALL_DIR) -- '$(1)/usr/sbin'
	$(INSTALL_BIN) -- '$(PKG_BUILD_DIR)/dir300-flash.sh' '$(1)/usr/sbin/dir300-flash'
	$(INSTALL_DIR) -- '$(1)/usr/share/dir300-flash'
	$(INSTALL_DATA) -- '$(DL_DIR)/ap61.ram' '$(1)/usr/share/dir300-flash'
	$(INSTALL_DATA) -- '$(DL_DIR)/ap61.rom' '$(1)/usr/share/dir300-flash'
	$(INSTALL_DATA) -- '$(PKG_BUILD_DIR)/dir300redboot.rom' '$(1)/usr/share/dir300-flash'
endef

$(eval $(call BuildPackage,dir300-flash))
$(eval $(call Download,ap61.ram))
$(eval $(call Download,ap61.rom))
$(eval $(call Download,dir300redboot.zip))
