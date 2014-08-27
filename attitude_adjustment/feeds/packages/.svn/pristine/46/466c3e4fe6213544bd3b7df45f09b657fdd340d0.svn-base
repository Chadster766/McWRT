#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=arm
PKG_VERSION:=1.4.5.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://archive.torproject.org/arm
PKG_MD5SUM:=f85f306e50b90796ab7097d948e8fcf2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/arm
  SECTION:=lang
  CATEGORY:=Languages
  SUBMENU:=Python
  TITLE:=Terminal status monitor for Tor relays
  URL:=http://www.atagar.com/arm/
  DEPENDS:=+python +python-ncurses +python-readline
endef

define Package/arm/description
  Command line application for monitoring Tor, providing real time status
  information such as the current configuration, bandwidth usage, message log,
  connections, etc. This uses a curses interface much like 'top' does for system
  usage. The application is intended for command-line aficionados, ssh
  connections, and anyone with a tty terminal for checking their relay's status.
endef

define Build/Compile
	$(call Build/Compile/PyMod,,install --prefix="/usr" --root="$(PKG_INSTALL_DIR)")
endef

define Package/arm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/arm $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/arm $(1)/usr/share/
endef

$(eval $(call BuildPackage,arm))
