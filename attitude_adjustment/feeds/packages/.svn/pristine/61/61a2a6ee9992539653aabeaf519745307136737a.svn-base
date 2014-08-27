#
# Copyright (C) 2011 Alexander Gordeev <lasaine@lvk.cs.msu.su>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bsdiff
PKG_VERSION:=4.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.daemonology.net/bsdiff
PKG_MD5SUM:=e6d812394f0e0ecc8d5df255aa1db22a

include $(INCLUDE_DIR)/package.mk

define Package/bsdiff
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libbz2
  URL:=http://www.daemonology.net/bsdiff
  TITLE:=Tools for building and applying patches to binary files
endef

define Package/bsdiff/description
 bsdiff and bspatch are tools for building and applying patches to binary
 files. By using suffix sorting (specifically, Larsson and Sadakane's
 qsufsort) and taking advantage of how executable files change, bsdiff
 routinely produces binary patches 50-80% smaller than those produced by
 Xdelta, and 15% smaller than those produced by .RTPatch (a $2750/seat
 commercial patch tool).
endef

define Package/bsdiff/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bsdiff $(PKG_BUILD_DIR)/bspatch $(1)/usr/bin/
endef

$(eval $(call BuildPackage,bsdiff))
