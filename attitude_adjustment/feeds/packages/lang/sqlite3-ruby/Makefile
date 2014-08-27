#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sqlite3-ruby
PKG_VERSION:=1.2.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://files.rubyforge.vm.bytemark.co.uk/sqlite-ruby/
PKG_MD5SUM:=fe75fa99dd9a2b5a2f84ab484e7f6469

include $(INCLUDE_DIR)/package.mk

define Package/ruby-sqlite3
  SUBMENU:=Ruby
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Ruby interface for SQLite
  URL:=http://www.ruby-lang.org/
  DEPENDS:=+libruby +libsqlite3 @BROKEN
endef

RUBY:= \
	ruby -I $(STAGING_DIR)/usr/share/ruby \

define Build/Configure
	(cd $(PKG_BUILD_DIR)/ext/sqlite3_api; \
		$(RUBY) extconf.rb; \
	)
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR)/ext/sqlite3_api; \
		$(MAKE); \
	)
endef

define Package/ruby-sqlite3/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/

endef

$(eval $(call BuildPackage,ruby-sqlite3))
