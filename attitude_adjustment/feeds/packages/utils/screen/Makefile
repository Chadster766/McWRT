#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=screen
PKG_VERSION:=4.0.3
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.uni-erlangen.de/pub/utilities/screen \
	@GNU/screen
PKG_MD5SUM:=8506fd205028a96c741e4037de6e3c42

include $(INCLUDE_DIR)/package.mk

define Package/screen
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses
  TITLE:=Full-screen terminal window manager
  URL:=http://www.gnu.org/software/screen/
endef

define Package/screen/description
	Screen is a full-screen window manager that multiplexes a physical
	terminal between several processes, typically interactive shells.
endef

define Build/Configure
	$(call Build/Configure/Default,\
		--with-sys-screenrc=/etc/screenrc,\
		LIBS="-lcrypt" \
	)
endef

define Package/screen/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/screen $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/etc/screenrc $(1)/etc/screenrc
endef

define Package/screen/conffiles
/etc/screenrc
endef

$(eval $(call BuildPackage,screen))
