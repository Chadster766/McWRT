#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libosip2
PKG_VERSION:=3.5.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/osip
PKG_MD5SUM:=7691546f6b3349d10007fc1aaff0f4e0

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libosip2
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU oSIP library
  URL:=http://www.gnu.org/software/osip/
  DEPENDS:=+libpthread
endef

define Package/libosip2description
 GNU oSIP library, a Session Initiation Protocol (SIP) implementation.
endef

TARGET_CFLAGS += $(FPIC)

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--disable-debug \
		--disable-trace \
		--enable-pthread \
		--enable-semaphore \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/osip{,parser}2 $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libosip{,parser}2.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libosip2.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libosip2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libosip{,parser}2.so $(PKG_INSTALL_DIR)/usr/lib/libosip{,parser}2.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libosip2))
