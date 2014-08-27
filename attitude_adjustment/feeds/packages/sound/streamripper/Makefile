#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=streamripper
PKG_VERSION:=1.62.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=1a8353af3c36d56f2d8ee8552f632071

PKG_INSTALL:=1
PKG_FIXUP:=autoreconf
PKG_LIBTOOL_PATHS:=. tre-0.7.2

include $(INCLUDE_DIR)/package.mk

define Package/streamripper
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=+libmad +libfaad2 +libpthread
  TITLE:=streamripper
  URL:=http://streamripper.sourceforge.net/
endef

define Package/streamripper/description
	Streamripper is an Open Source application that lets you record streaming mp3 to your hard drive.
endef


EXTRA_CFLAGS += $(TARGET_CPPFLAGS)

CONFIGURE_ARGS+= \
	--without-ogg \
	--with-included-argv \
	--without-included-libmad \
	--without-libiconv-prefix \
	--with-included-tre \
	--with-gnu-ld \

define Package/streamripper/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/streamripper $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/agrep $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtre.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,streamripper))
