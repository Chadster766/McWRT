#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aMule
PKG_VERSION:=2.2.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/amule
PKG_MD5SUM:=530d9b48187e36f78fc21bb19e94326d

PKG_BUILD_DEPENDS:=libgd

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/amule
  SUBMENU:=P2P
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A multi-platform eMule-like ed2k client
  URL:=http://www.amule.org/
  DEPENDS:=+libpng +libpthread +libncurses +libreadline +libwxbase +libcryptoxx
endef

CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-static \
	--disable-rpath \
	--with-gnu-ld \
	--disable-ccache \
	--disable-debug \
	--disable-optimize \
	--disable-profile \
	--disable-monolithic \
	--enable-amule-daemon \
	--enable-amulecmd \
	--disable-amulecmdgui \
	--enable-webserver \
	--disable-webservergui \
	--disable-amule-gui \
	--disable-cas \
	--disable-wxcas \
	--disable-ed2k \
	--disable-alc \
	--disable-alcc \
	--disable-systray \
	--disable-utf8-systray \
	--enable-embedded-crypto \
	--enable-gsocket \
	--disable-gtktest \
	--disable-crypto \
	\
	--with-zlib="$(STAGING_DIR)/usr" \
	--with-gdlib-prefix="$(STAGING_DIR)/usr" \
	--with-libpng-prefix="$(STAGING_DIR)/usr" \
	--with-wx-prefix="$(STAGING_DIR)/usr" \
	--with-crypto-prefix="$(STAGING_DIR)/usr" \
	--with-libiconv-prefix="$(ICONV_PREFIX)" \
	--with-libintl-prefix="$(INTL_PREFIX)" \
	--without-x \

TARGET_LDFLAGS += \
	-liconv

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		HOSTCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/amule/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/amule{cmd,d,web} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/amule $(1)/usr/share/
endef

$(eval $(call BuildPackage,amule))
