#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=click
PKG_VERSION:=20101020
PKG_RELEASE:=1
PKG_REV:=8f521a2dcd58ea8205b3b6070be605a21fb9154b

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://read.cs.ucla.edu/git/click
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_MD5SUM:=c3cba1c923d0a034cc518720b5ba7b40

include $(INCLUDE_DIR)/package.mk

define Package/click
	SECTION:=net
	CATEGORY:=Network
	DEPENDS:=@BROKEN
	TITLE:=Click Modular Router
	URL:=http://read.cs.ucla.edu/click
endef

define Package/click/Description
	The Click Modular Router userspace package
endef

TARGET_CFLAGS += "-static -O2 -MD"
TARGET_CXXFLAGS += "-static -O2 -MD"

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		rm -rf config.{cache,status} ; \
		./configure \
			--prefix=/usr \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_HOST_NAME) \
			--build=$(GNU_BUILD_NAME) \
			--enable-tools=mixed \
			--enable-userlevel \
			--enable-wifi \
			--enable-fixincludes \
			--disable-linuxmodule \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		tools elementmap.xml
	(cd $(PKG_BUILD_DIR)/userlevel; \
		../tools/click-mkmindriver/click-mkmindriver -p $(PKG_NAME) -C .. \
		-f $(PKG_BUILD_DIR)/conf/wifi/dump.click \
		-A --all -E Discard -E Print; \
	)
	$(MAKE) -C $(PKG_BUILD_DIR) MINDRIVER=$(PKG_NAME)
endef

define Package/click/install
	$(INSTALL_DIR) $(1)/usr
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/share/click
	$(CP) $(PKG_BUILD_DIR)/userlevel/$(PKG_NAME)click $(1)/usr/bin/click
	$(CP) $(PKG_BUILD_DIR)/tools/click-align/click-align $(1)/usr/bin/click-align
	$(CP) $(PKG_BUILD_DIR)/elementmap.xml $(1)/usr/share/click/elementmap.xml
endef

$(eval $(call BuildPackage,click))
