#
# Copyright (C) 2011 Alexander Gordeev <lasaine@lvk.cs.msu.su>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xdelta3
PKG_VERSION:=3.0.0
PKG_RELEASE:=1

PKG_SOURCE:=xdelta$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://xdelta.googlecode.com/files
PKG_MD5SUM:=5fe038be3a266d2a7913e10d1cec6d88

PKG_BUILD_DIR:=$(BUILD_DIR)/xdelta$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/xdelta3
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://xdelta.org
  TITLE:=A diff utility which works with binary files
endef

define Package/xdelta3/description
 Xdelta3 is a set of tools designed to compute changes between binary
 files. These changes (delta files) are similar to the output of the
 "diff" program, in that they may be used to store and transmit only
 the changes between files. The "delta files" that Xdelta3 manages are
 stored in RFC3284 (VCDIFF) format.
endef

define Package/xdelta3/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xdelta3 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,xdelta3))
