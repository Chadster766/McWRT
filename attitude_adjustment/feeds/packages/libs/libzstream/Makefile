#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libzstream
PKG_VERSION:=2012-06-11
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://dev.freecwmp.org/zstream-clone/
# PKG_SOURCE_URL:=git://midlink.org/zstream.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=3b9748999aa3d81a5811eff1cc221266a2588e28
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/libzstream
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=minimalistic network stream library
  DEPENDS:=+libubox +libopenssl
endef

define Package/zstream
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=File Transfer
  TITLE:=minimalistic network stream client
  DEPENDS:=+libzstream
endef

TARGET_LDFLAGS += \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/zstream.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/include/zstream
	$(CP) $(PKG_BUILD_DIR)/zstream/http.h $(1)/usr/include/zstream
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libzstream.so $(1)/usr/lib
endef

define Package/libzstream/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/libzstream.so $(1)/usr/lib
endef

define Package/zstream/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/cli/zstream $(1)/usr/bin
endef

$(eval $(call BuildPackage,libzstream))
$(eval $(call BuildPackage,zstream))
