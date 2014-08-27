#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sqlite2
PKG_VERSION:=2.8.17
PKG_RELEASE:=2

PKG_SOURCE:=sqlite-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.sqlite.org/
PKG_MD5SUM:=838dbac20b56d2c4292e98848505a05b

PKG_BUILD_DIR:=$(BUILD_DIR)/sqlite-$(PKG_VERSION)

PKG_BUILD_DEPENDS:=libncurses libreadline

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/sqlite2/Default
  SUBMENU:=database
  TITLE:=SQLite (v2.x) database engine
  URL:=http://www.sqlite.org/
endef

define Package/libsqlite2
  $(call Package/sqlite2/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
endef

define Package/libsqlite2/description
	SQLite is a small C library that implements a self-contained, embeddable, 
	zero-configuration SQL database engine.	
	This package contains the SQLite (v2.x) shared library, used by other programs.
endef

define Package/sqlite2-cli
  $(call Package/sqlite2/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libsqlite2 +libncurses +libreadline
  TITLE+= (cli)
  SUBMENU:=database
endef

define Package/sqlite2-cli/description
	SQLite is a small C library that implements a self-contained, embeddable, 
	zero-configuration SQL database engine.	
	This package contains a terminal-based front-end to the SQLite (v2.x) library 
	that can evaluate queries interactively and display the results in 
	multiple formats.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		, \
		config_BUILD_CC="$(HOSTCC)" \
		config_BUILD_CFLAGS="-O2" \
		config_TARGET_CC="$(TARGET_CC)" \
		config_TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		config_TARGET_READLINE_INC="$(TARGET_CPPFLAGS)" \
		config_TARGET_READLINE_LIBS="$(TARGET_LDFLAGS) -lreadline -lncurses" \
		config_TARGET_TCL_INC="-DNO_TCL=1" \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sqlite.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/sqlite.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libsqlite2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite.so.* $(1)/usr/lib/
endef

define Package/sqlite2-cli/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sqlite $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libsqlite2))
$(eval $(call BuildPackage,sqlite2-cli))
