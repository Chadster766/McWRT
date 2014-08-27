# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mc
PKG_VERSION:=4.7.5.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.midnight-commander.org/downloads/
PKG_MD5SUM:=6a67e139e0032d8a871455a80d490941

PKG_BUILD_PARALLEL:=1
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/mc
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+glib2 +libncurses $(LIBRPC_DEPENDS) $(ICONV_DEPENDS)
  TITLE:=midnight commander - a powerful file manager
  URL:=http://www.midnight-commander.org/
  SUBMENU:=filemanager
  MAINTAINER:=Luka Perkov <openwrt@lukaperkov.net>
  MENU:=1
endef

define Package/mc/config
	source "$(SOURCE)/Config.in"
endef

define Package/mc/description
 GNU Midnight Commander is a visual file manager, licensed under GNU General
 Public License and therefore qualifies as Free Software. It's a feature rich
 full-screen text mode application that allows you to copy, move and delete
 files and whole directory trees, search for files and run commands in the
 subshell.
endef

CONFIGURE_ARGS += \
	--enable-utf8 \
	--disable-doxygen-doc \
	--with-screen=ncurses \
	--without-gpm-mouse \
	--without-x \
	ac_cv_search_addwstr=no

ifeq ($(CONFIG_MC_DIFF_VIEWER),n)
CONFIGURE_ARGS += \
	--without-diff-viewer
endif

ifeq ($(CONFIG_MC_EDITOR),n)
CONFIGURE_ARGS += \
	--without-edit
endif

ifeq ($(CONFIG_MC_SUBSHELL),n)
CONFIGURE_ARGS += \
	--without-subshell
endif

ifeq ($(CONFIG_MC_DISABLE_VFS),y)
CONFIGURE_ARGS += \
	--without-vfs
endif

define Package/mc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mc $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/mc
ifeq ($(CONFIG_MC_DIFF_VIEWER),y)
	ln -sf mc $(1)/usr/bin/mcdiff
endif
ifeq ($(CONFIG_MC_EDITOR),y)
	ln -sf mc $(1)/usr/bin/mcedit
endif
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/misc/mc.menu $(1)/etc/mc
	$(INSTALL_DIR) $(1)/etc/mc/skins
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/misc/skins/default.ini $(1)/etc/mc/skins
	$(INSTALL_DIR) $(1)/root/.mc/cedit/Syntax
endef

define Package/mc/conffiles
/etc/mc/mc.menu
/etc/mc/skins/default.ini
endef

$(eval $(call BuildPackage,mc))
