#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:= electric-fence
PKG_VERSION:=2.1.13
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION)-0.1.tar.gz
PKG_SOURCE_URL:=http://perens.com/FreeSoftware/ElectricFence
PKG_MD5SUM:=59e4e7817a30aff52d8971ce00e1ad35

include $(INCLUDE_DIR)/package.mk

define Package/electric-fence
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=electric-fence
  URL:=http://perens.com/FreeSoftware/ElectricFence/
  DEPENDS:=@USE_EGLIBC
  MAINTAINER:=Mirko Vogt <mirko@openwrt.org>
endef

TARGET_CFLAGS += -fPIC
TARGET_LDFLAGS += -lpthread

define Build/Compile
	rm -f $(PKG_BUILD_DIR)/libefence.so $(PKG_BUILD_DIR)/libefence.a
	$(TARGET_CC) $(TARGET_CFLAGS) -c $(PKG_BUILD_DIR)/efence.c -o $(PKG_BUILD_DIR)/efence.o
	$(TARGET_CC) $(TARGET_CFLAGS) -c $(PKG_BUILD_DIR)/page.c -o $(PKG_BUILD_DIR)/page.o
	$(TARGET_CC) $(TARGET_CFLAGS) -c $(PKG_BUILD_DIR)/print.c -o $(PKG_BUILD_DIR)/print.o
	$(TARGET_CC) -shared $(TARGET_LDFLAGS) -o $(PKG_BUILD_DIR)/libefence.so $(PKG_BUILD_DIR)/efence.o $(PKG_BUILD_DIR)/page.o $(PKG_BUILD_DIR)/print.o
	$(AR) crv $(LD_FLAGS) $(PKG_BUILD_DIR)/libefence.a $(PKG_BUILD_DIR)/efence.o $(PKG_BUILD_DIR)/page.o $(PKG_BUILD_DIR)/print.o
endef

define Build/InstallDev
	$(INSTALL_DIR) \
		$(1)/usr/lib

	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/libefence.so \
		$(1)/usr/lib/

	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/libefence.a \
		$(1)/usr/lib/
endef

define Package/electric-fence/install
	$(INSTALL_DIR) \
		$(1)/usr/lib

	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/libefence.so \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,electric-fence))
