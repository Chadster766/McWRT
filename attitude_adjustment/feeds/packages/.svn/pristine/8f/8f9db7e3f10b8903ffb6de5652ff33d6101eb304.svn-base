#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dsniff
PKG_VERSION:=2.4b1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.monkey.org/~dugsong/dsniff/beta
PKG_MD5SUM:=2f761fa3475682a7512b0b43568ee7d6

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-2.4

include $(INCLUDE_DIR)/package.mk

define Package/dsniff
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libnids +libnet0 +libopenssl +libgdbm $(LIBRPC_DEPENDS) @BROKEN
  TITLE:=A collection of tools for network auditing and penetration testing
  URL:=http://www.monkey.org/~dugsong/dsniff/
endef

define Package/dsniff/description
	A collection of tools for network auditing and penetration testing.
endef

TARGET_LDFLAGS += $(LIBRPC)

define Build/Configure
	$(call Build/Configure/Default, \
		--with-gdbm="$(STAGING_DIR)/usr" \
		--with-libnids="$(STAGING_DIR)/usr" \
		--with-libpcap="$(STAGING_DIR)/usr" \
		--with-libnet="$(STAGING_DIR)/usr/lib/libnet-1.0.x" \
		--with-openssl="$(STAGING_DIR)/usr" \
		--without-db \
		--without-x \
	)
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		install_prefix="$(PKG_INSTALL_DIR)" \
		all install
endef

define Package/dsniff/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)  $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,dsniff))
