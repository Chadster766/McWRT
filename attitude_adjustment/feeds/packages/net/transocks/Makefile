#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=transocks
PKG_VERSION:=1.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=:pserver:anonymous@transocks.cvs.sourceforge.net:/cvsroot/transocks
PKG_SOURCE_SUBDIR:=transocks
PKG_SOURCE_VERSION:=-DNOW
PKG_SOURCE_PROTO:=cvs

include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) --strip-components=1 -xf -

define Build/Compile
	$(TARGET_CC) \
		-I$(STAGING_DIR)/usr/include \
		$(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		$(LIBRPC) -lsocks -o \
		$(PKG_BUILD_DIR)/transocks \
		$(PKG_BUILD_DIR)/transocks.c
endef

define Package/transocks
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=Transparent SOCKSifying proxy
  DEPENDS:=+libsocks $(LIBRPC_DEPENDS)
endef

define Package/transocks/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/transocks $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,transocks))
