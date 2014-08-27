#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ettercap
PKG_VERSION:=NG-0.7.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/ettercap
PKG_MD5SUM:=28fb15cd024162c55249888fe1b97820

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/ettercap
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libnet1 +libopenssl +libpcre +libncurses +libltdl +libpthread +zlib
  TITLE:=Ettercap is a suite for man in the middle attacks on LAN.
  URL:=http://ettercap.sourceforge.net
endef

define Package/ettercap/description
	It features sniffing of live connections, content filtering on the fly 
	and many other interesting tricks. 
	It supports active and passive dissection of many protocols (even 
	ciphered ones) and includes many feature for network and host analysis.
endef

define Package/ettercap/conffiles
/etc/etter.conf
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	# remove default optimization flags set by configure
	$(SED) 's,DEBUG_FLAGS=.*,DEBUG_FLAGS=,g' $(PKG_BUILD_DIR)/configure
endef

CONFIGURE_ARGS += \
	--with-libpcap="$(STAGING_DIR)/usr" \
	--with-libnet="$(STAGING_DIR)/usr/lib/libnet-1.1.x" \
	--with-openssl="$(STAGING_DIR)/usr" \
	--with-libpcre="$(STAGING_DIR)/usr" \
	--with-iconv="$(ICONV_PREFIX)" \
	--with-libncurses="$(STAGING_DIR)/usr" \
	--disable-gtk \
	--disable-debug \
	--enable-plugins

MAKE_FLAGS += \
	DESTDIR="$(PKG_INSTALL_DIR)"

define Package/ettercap/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/etter.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/etter{cap,filter,log} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib/ettercap
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/ettercap/*.so $(1)/usr/lib/ettercap/
	$(INSTALL_DIR) $(1)/usr/share/ettercap
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/share/ettercap/* $(1)/usr/share/ettercap/
endef

$(eval $(call BuildPackage,ettercap))
