#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pwgen
PKG_VERSION:=2.06
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=935aebcbe610fbc9de8125e7b7d71297

include $(INCLUDE_DIR)/package.mk

define Package/pwgen
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Automatic Password generator
  URL:=http://sourceforge.net/projects/pwgen/
endef

define Package/pwgen/description
	pwgen generates random, meaningless but pronounceable passwords.
	These passwords contain either only lowercase letters, or upper
	and lower case mixed, or digits thrown in.
	Uppercase letters and digits are placed in a way that eases
	remembering their position when memorizing only the word.

	E.g. to generate a WPA preshared-key do:
	pwgen -c -n -s 63 1
endef

# uses GNU configure

define Package/pwgen/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pwgen $(1)/usr/bin/
endef

$(eval $(call BuildPackage,pwgen))
