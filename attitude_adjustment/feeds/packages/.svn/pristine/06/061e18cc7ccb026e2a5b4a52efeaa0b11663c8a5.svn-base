#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=poco
PKG_VERSION:=1.3.6p2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.sourceforge.net/project/poco/sources/poco-1.3.6
PKG_MD5SUM:=32d154ca5f34d20217c0baaffa432c1c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/poco
  NAME:=poco
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Poco C++ libraries
  URL:=http://www.pocoproject.org/
  DEPENDS:=+libstdcpp +libpthread +librt
endef

define Package/poco/description
  Modern, powerful open source C++ class libraries and frameworks for building
  network- and internet-based applications that run on desktop, server and
  embedded systems.
endef

CONFIGURE_ARGS += \
	--config=Linux \
	--no-tests \
	--no-samples \
	--no-fpenvironment \
	--no-sharedmemory \
	--no-wstring

define Package/poco/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libPoco{Foundation,XML,Net,Util}.so* $(1)/usr/lib/
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) -r $(PKG_INSTALL_DIR)/usr/include/Poco $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libPoco{Foundation,XML,Net,Util}.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,poco))
