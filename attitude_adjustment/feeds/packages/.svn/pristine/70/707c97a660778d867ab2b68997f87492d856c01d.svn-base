#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sigscheme
PKG_VERSION:=0.8.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://sigscheme.googlecode.com/files/
PKG_MD5SUM:=f25b0acbb854651b375b42f17ad20f39

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/sigscheme
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=SigScheme
  URL:=http://code.google.com/p/sigscheme/
  DEPENDS:=@!avr32
endef

define Package/sigscheme/description
SigScheme is a R5RS Scheme interpreter for embedded use. It features small
footprint (64KB in library form on the 'small' configuration) like SIOD and
TinyScheme, low memory consumption (2-words per cons cell), multibyte characters
handling (UTF-8, EUCs and Shift_JIS) and more.
endef

define Package/sigscheme/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{gcroots,sscm}.so.* $(1)/usr/lib

	$(INSTALL_DIR) $(1)/usr/share/sigscheme/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/share/sigscheme/lib/* \
		$(1)/usr/share/sigscheme/lib/

	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/sscm $(1)/usr/bin
endef

$(eval $(call BuildPackage,sigscheme))
