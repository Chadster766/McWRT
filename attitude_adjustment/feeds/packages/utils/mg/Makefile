#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mg
PKG_VERSION:=20110120
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://homepage.boetes.org/software/mg
PKG_MD5SUM:=f2fa8016a491aa2a9d4c5698c593f31b

include $(INCLUDE_DIR)/package.mk

define Package/mg
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=microscopic GNU Emacs-style editor
  URL:=http://homepage.boetes.org/software/mg/
  SUBMENU:=Editors
endef

define Package/mg/description
	This program is intended to be a small, fast, and portable
	editor for people who can't (or don't want to) run real
	Emacs for one reason or another.  It is compatible with GNU
	because there shouldn't be any reason to learn more than
	one Emacs flavor.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
	);
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		LDFLAGS="$(TARGET_LDFLAGS) -lncurses" \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		CC="$(TARGET_CC)"
endef

define Package/mg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mg))
