#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=check
PKG_VERSION:=0.9.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/check
PKG_MD5SUM:=5d75e9a6027cde79d2c339ef261e7470

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/check
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Unit testing framework for C
  URL:=http://check.sourceforge.net/
  DEPENDS:= +libpthread
endef

define Package/check/description
  Check features a simple interface for defining unit tests, putting little in
  the way of the developer. Tests are run in a separate address space, so Check
  can catch both assertion failures and code errors that cause segmentation
  faults or other signals. The output from unit tests can be used within source
  code editors and IDEs.
endef

define Build/Configure
	$(CP) $(SCRIPT_DIR)/config.* $(PKG_BUILD_DIR)/build-aux/
	$(call Build/Configure/Default)
endef

define Build/Install
	$(call Build/Install/Default)
	cd $(PKG_INSTALL_DIR)/usr/lib/ ; ln -nsf libcheck.so libcheck_pic.so
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/check/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcheck.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,check))
