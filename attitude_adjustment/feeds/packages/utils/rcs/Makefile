# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rcs
PKG_VERSION:=5.7
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=4c8e896f2d2446fa593c6f1601a4fb75

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=The GNU Revision Control System
  URL:=http://www.gnu.org/software/$(PKG_NAME)/
endef

define Package/$(PKG_NAME)/desctiption
 The Revision Control System (RCS) manages multiple revisions of files. RCS
 automates the storing, retrieval, logging, identification, and merging of
 revisions. RCS is useful for text that is revised frequently, for example
 programs, documentation, graphics, papers, and form letters.
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/{ci,co,ident,merge,rcs,rcsclean,rcsdiff,rcsmerge,rlog} $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/rcsfreeze.sh $(1)/usr/bin/rcsfreeze
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
