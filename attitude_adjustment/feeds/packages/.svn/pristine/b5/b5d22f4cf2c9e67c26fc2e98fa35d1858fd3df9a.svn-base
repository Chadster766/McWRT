#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sdparm
PKG_VERSION:=1.04
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://sg.danny.cz/sg/p
PKG_MD5SUM:=f4b8c7bfc1cd27d30fbdc066a65a44ea

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/sdparm
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Read or modify SCSI or USB disk parameters
  DEPENDS:=+kmod-scsi-generic
  SUBMENU:=disc
  URL:=http://sg.danny.cz/sg/sdparm.html
endef

define Package/sdparm/description
	The sdparm utility outputs and in some cases modifies SCSI device
	parameters.
endef

define Package/sdparm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,sdparm))
