#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=huaweiaktbbo
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/huaweiaktbbo
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libusb
  TITLE:=Huawei E156/E169/E220 activation tool
endef

define Build/Prepare	
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-lusb \
		-o $(PKG_BUILD_DIR)/huaweiAktBbo \
		$(PKG_BUILD_DIR)/huaweiAktBbo.c
endef

define Package/huaweiaktbbo/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/huaweiAktBbo $(1)/usr/bin/
endef

$(eval $(call BuildPackage,huaweiaktbbo))
