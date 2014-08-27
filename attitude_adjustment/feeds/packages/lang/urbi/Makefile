#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=urbi
PKG_VERSION:=1.0RC2-l455
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-sdk-$(PKG_VERSION)-src.zip
PKG_SOURCE_URL:=http://www.gostai.com/downloads/aibo/
PKG_MD5SUM:=02f64abb6232ddd7b0cb6dc182e2df26
PKG_FIXUP:=libtool-ucxx

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-sdk

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/urbi
  SECTION:=lang
  CATEGORY:=Languages
  DEPENDS:=+uclibcxx +libpthread @BROKEN
  TITLE:=URBI SDK
  URL:=http://www.urbiforge.com
endef

define Package/urbi/description
  Universal Real-Time Behavior Interface for robots.
endef

CONFIGURE_VARS += \
	CXX="g++-uc" \
	CXXFLAGS="$$$$CXXFLAGS -fno-rtti" \
	EXAMPLES=0 \

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		WARNING_CXXFLAGS="-Wall" \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/urbi
	$(CP) $(PKG_INSTALL_DIR)/usr/include/urbi* $(1)/usr/include/urbi/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/liburbi* $(1)/usr/lib/
endef

define Package/urbi/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/gostai
	$(CP) $(PKG_INSTALL_DIR)/usr/gostai/* $(1)/usr/gostai/
endef

$(eval $(call BuildPackage,urbi))
