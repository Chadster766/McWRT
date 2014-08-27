#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nprobe
PKG_VERSION:=4.1
PKG_RELEASE:=2

PKG_SOURCE:=nProbe-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources
PKG_MD5SUM:=32ed12f60eab771a79bd5f90622fa2a3

BUILD_DEPENDS:=libwrap
PKG_FIXUP:=autoreconf

PKG_BUILD_DIR:=$(BUILD_DIR)/nProbe

include $(INCLUDE_DIR)/package.mk

define Package/nprobe
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libpcap +libpcre +libpthread +NPROBE_MYSQL_SUPPORT:libmysqlclient +zlib
  TITLE:=nProbe
  URL:=http://invisible-island.net/dialog/
endef

define Package/nprobe/config
	config NPROBE_MYSQL_SUPPORT
		depends on PACKAGE_nprobe
		bool "MySQL support"

endef

define Package/nprobe/description
		nprobe
endef

ifndef CONFIG_NPROBE_MYSQL_SUPPORT
  CONFIGURE_VARS += ac_cv_prog_MYSQL=no
endif

define Build/Configure
	# patch creates these, so we make them executable
	chmod +x $(PKG_BUILD_DIR)/configure
	chmod +x $(PKG_BUILD_DIR)/config.guess
	$(call Build/Configure/Default, \
		--with-only-ipv4 \
		PCAP_ROOT="$(STAGING_DIR)/usr" \
	)
endef

define Build/Compile
	DESTDIR=$(PKG_INSTALL_DIR) \
		LDFLAGS="$(TARGET_LDFLAGS) -ldl" \
		$(MAKE) -C $(PKG_BUILD_DIR) \
		all install
endef

define Package/nprobe/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/nprobe \
		$(1)/usr/bin/
	$(CP) -a $(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib
endef

$(eval $(call BuildPackage,nprobe))

