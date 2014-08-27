# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=liblo
PKG_VERSION:=0.26
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/liblo
PKG_MD5SUM:=5351de14262560e15e7f23865293b16f

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/liblo/Default
  SUBMENU:=Sound
  TITLE:=Lightweight Open Sound Control (OSC)
  URL:=http://liblo.sourceforge.net/
endef

define Package/liblo
$(call Package/liblo/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= library
  DEPENDS:= +libpthread
endef

define Package/liblo-utils
$(call Package/liblo/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= utilities
  DEPENDS:= +liblo
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/include/lo \
		$(1)/usr/include/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/liblo.so* \
		$(1)/usr/lib/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/pkgconfig/liblo.pc \
		$(1)/usr/lib/pkgconfig/ \
	)
endef

define Package/liblo/install
	$(INSTALL_DIR) $(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/liblo.so.* \
		$(1)/usr/lib/ \
	)
endef

define Package/liblo-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/bin/oscdump \
		./usr/bin/oscsend \
		$(1)/usr/bin/ \
	)
endef

$(eval $(call BuildPackage,liblo))
$(eval $(call BuildPackage,liblo-utils))
