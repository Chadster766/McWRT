#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=remotefs
PKG_REV:=1297
PKG_VERSION:=$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_SUBDIR:=remotefs-$(PKG_VERSION)
PKG_SOURCE_URL:=http://remotefs.svn.sourceforge.net/svnroot/remotefs/trunk
PKG_SOURCE:=$(PKG_SOURCE_SUBDIR).tar.gz
include $(INCLUDE_DIR)/package.mk

define Package/remotefs
  TITLE:=RemoteFS
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Filesystem
  DEPENDS:=+libfuse +libpthread
  URL:=http://remotefs.sourceforge.net
endef

define Package/remotefs/description
  remotefs is a network file system designed for use with home NAS.
  Simple to use, few dependencies, reasonable security, completely
  in user space. Provided remotefs-server package contains server
  components.
endef

define Package/remotefs-server
  $(call Package/remotefs)
  TITLE:=RemoteFS server
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		INSTALL_DIR="$(PKG_INSTALL_DIR)" \
		HOST_CPU="$(ARCH)" \
		CFLAGS_RELEASE="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS_RELEASE="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib" \
		server client install_client install_server
endef

define Package/remotefs/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rfs $(1)/bin/rfs
	$(INSTALL_DIR) $(1)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/librfs.so.* $(1)/lib
endef

define Package/remotefs-server/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rfsd $(1)/bin/rfsd
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rfspasswd $(1)/bin/rfspasswd
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/build/etc/rfs-exports $(1)/etc/rfs-exports
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rfsd $(1)/etc/init.d/rfsd
endef

define Package/remotefs-server/conffiles
/etc/rfs-exports
endef

$(eval $(call BuildPackage,remotefs))
$(eval $(call BuildPackage,remotefs-server))
