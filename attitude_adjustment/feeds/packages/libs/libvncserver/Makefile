#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=LibVNCServer
PKG_VERSION:=0.9.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libvncserver
PKG_MD5SUM:=14af5bdae461df4666c18e5f83c150c4
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

PKG_INSTALL=1

define Package/libvncserver
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+zlib +libjpeg +libpthread $(ICONV_DEPENDS)
  TITLE:=VNC Server Library
  URL:=http://libvncserver.sourceforge.net/
endef

CONFIGURE_ARGS += \
	--without-xkeyboard \
	--without-xinerama \
	--without-xrandr \
	--without-xfixes \
	--without-xdamage \
	--without-xtrap \
	--without-xrecord \
	--without-fbpm \
	--without-dpms \
	--without-filetransfer \
	--without-macosx-native \
	--without-ssl \
	--without-avahi

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/{lib,include,bin}
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib* $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
endef

define Package/libvncserver/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib*so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libvncserver))
