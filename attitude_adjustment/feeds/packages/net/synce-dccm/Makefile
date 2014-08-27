#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=synce-dccm
PKG_VERSION:=0.9.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/synce
PKG_MD5SUM:=8818b71133049fe9c739166225aebe0c

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/synce-dccm
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libsynce $(ICONV_DEPENDS)
  TITLE:=dccm for mobiles
  URL:=http://sourceforge.net/projects/synce/
endef

define Package/synce-dccm/description
Description TBD
endef

CONFIGURE_VARS+= \
	CPPFLAGS="$$$$CPPFLAGS -I$(ICONV_PREFIX)/include" \
	CPPFLAGS="$$$$CPPFLAGS -I$(INTL_PREFIX)/include" \
	CPPFLAGS="$$$$CPPFLAGS -Wl,-rpath-link,$(STAGING_DIR)/usr/lib:$(ICONV_PREFIX)/lib:$(INTL_PREFIX)/lib" 

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	        DESTDIR="$(PKG_INSTALL_DIR)" \
		LDFLAGS="$(TARGET_LDFLAGS) -L$(ICONV_PREFIX)/lib" \
		LIBS="-lsynce -liconv" \
		all install
endef

define Package/synce-dccm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dccm $(1)/usr/bin/
endef

$(eval $(call BuildPackage,synce-dccm))
#LIBS="-Wl,-Bstatic,-lsynce,-Bdynamic,-liconv" \
