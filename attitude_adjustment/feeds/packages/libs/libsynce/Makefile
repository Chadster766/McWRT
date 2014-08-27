#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsynce
PKG_VERSION:=0.12
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/synce
PKG_MD5SUM:=fd473d3deceda7912af4427dede1736f

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1
PKG_FIXUP:=autoreconf

define Package/libsynce
  DEPENDS:=$(ICONV_DEPENDS) $(INTL_DEPENDS)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=SynCE library
  URL:=http://synce.sourceforge.net/
endef

define Package/libsynce/description
The purpose of the SynCE project is to provide a means of communication with a
Windows CE or Pocket PC device from a computer running Linux, *BSD or other unices.
endef

MAKE_FLAGS += \
	OPTIM="$(TARGET_CFLAGS)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	DESTDIR="$(PKG_INSTALL_DIR)"

CONFIGURE_ARGS += \
	--enable-hal-support=no \
	--enable-odccm-support=no

CONFIGURE_VARS+= \
	CPPFLAGS="$$$$CPPFLAGS -I$(ICONV_PREFIX)/include" \
	CPPFLAGS="$$$$CPPFLAGS -I$(INTL_PREFIX)/include" \
	LDFLAGS="$$$$LDFLAGS -L$(ICONV_PREFIX)/lib" \
	LDFLAGS="$$$$LDFLAGS -L$(INTL_PREFIX)/lib" \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsynce.so* $(1)/usr/lib/
endef

define Package/libsynce/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsynce.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsynce))
