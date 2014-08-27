#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
 
include $(TOPDIR)/rules.mk

PKG_NAME:=boxbackup
PKG_VERSION:=0.11.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.boxbackup.org/trac/export/2998/box/packages
PKG_MD5SUM:=c4c4786a918a9c026f80833614c40a6b

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/boxbackup/Default
  SUBMENU:=backup
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://www.boxbackup.org/
  MAINTAINER:=Daniel Dickinson <openwrt@cshore.neomailbox.net>
endef

define Package/boxbackup
  $(call Package/boxbackup/Default)
  TITLE:=Secure network backup
  MENU:=1
endef

define Package/bbstored
  $(call Package/boxbackup/Default)
  TITLE:=Server for secure network backup
  DEPENDS:=boxbackup +libopenssl +zlib +libdb47 +libopenssl +libstdcpp +libreadline +libncurses
endef

define Package/bbstored/description
  Boxbackup is an open source, completely automatic, on-line backup system. 
  This package is for the server (where the backups are stored).  The
  server is known as bbstored.
endef

define Package/bbstored/conffiles
/etc/config/bbbstored
endef


define Package/bbstored-config-external
  $(call Package/boxbackup/Default)
  TITLE:=Configure network backup server
  DEPENDS:=boxbackup +openssl-util +microperl
endef

define Package/boxbackup-certs
  $(call Package/boxbackup/Default)
  TITLE:=Manage network backup security certificates
  DEPENDS:=boxbackup +openssl-util +microperl
endef

define Package/bbackupd
  $(call Package/boxbackup/Default)
  TITLE:=Client for secure network backup
  DEPENDS:=boxbackup +libopenssl +zlib +libdb47 +libopenssl +libreadline +libstdcpp +libncurses +openssl-util +microperl
endef

define Package/bbackupd/description
  Box Backup is an open source, completely automatic, on-line backup system. 
  This package is for the client (host to be backed up).
endef


TARGET_CFLAGS += $(FPIC) -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib

CONFIGURE_ARGS += \
	--sysconfdir=/var/etc \
	--enable-gnu-readline \
	--with-bdb-headers=$(STAGING_DIR)/usr/include \
	--with-bdb-lib=$(STAGING_DIR)/usr/lib \
	--with-ssl-header=$(STAGING_DIR)/usr/include \
	--with-ssl-lib=$(STAGING_DIR)/usr/lib \
	--with-random=/dev/urandom \
	--disable-static-bin

define Build/Install
	mkdir -p $(PKG_INSTALL_DIR)
	tar -C $(PKG_INSTALL_DIR) -xzf $(PKG_BUILD_DIR)/parcels/$(PKG_NAME)-$(PKG_VERSION)-backup-server-linux-gnu.tgz 
	tar -C $(PKG_INSTALL_DIR) -xzf $(PKG_BUILD_DIR)/parcels/$(PKG_NAME)-$(PKG_VERSION)-backup-client-linux-gnu.tgz
endef

BXBK_SRV_INSTALL_DIR = $(PKG_INSTALL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-backup-server-linux-gnu
BXBK_CLIENT_INSTALL_DIR = $(PKG_INSTALL_DIR)/$(PKG_NAME)-$(PKG_VERSION)-backup-client-linux-gnu

define Package/boxbackup/install
	true
endef

define Package/bbstored/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(BXBK_SRV_INSTALL_DIR)/bbstored $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/boxbackup	
	$(INSTALL_BIN) $(BXBK_SRV_INSTALL_DIR)/bbstoreaccounts $(1)/usr/lib/boxbackup/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/bbstored.init $(1)/etc/init.d/bbstored
	$(INSTALL_DIR) $(1)/lib/functions
	$(INSTALL_DATA) ./files/bbstored.sh $(1)/lib/functions/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/bbstored.config $(1)/etc/config/bbstored
	$(INSTALL_BIN) ./files/bbstoreaccounts $(1)/usr/sbin/
endef

define Package/bbstored-config-external/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(BXBK_SRV_INSTALL_DIR)/bbstored-config $(1)/usr/sbin/
	$(INSTALL_BIN) $(BXBK_SRV_INSTALL_DIR)/raidfile-config $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/bbstored
endef

define Package/boxbackup-certs/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(BXBK_SRV_INSTALL_DIR)/bbstored-certs $(1)/usr/sbin/
endef

define Package/bbackupd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(BXBK_CLIENT_INSTALL_DIR)/bbackupd $(1)/usr/sbin/
	$(INSTALL_BIN) $(BXBK_CLIENT_INSTALL_DIR)/bbackupquery $(1)/usr/sbin/
	$(INSTALL_BIN) $(BXBK_CLIENT_INSTALL_DIR)/bbackupctl $(1)/usr/sbin/
	$(INSTALL_BIN) $(BXBK_CLIENT_INSTALL_DIR)/bbackupd-config $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,boxbackup))
$(eval $(call BuildPackage,bbstored))
$(eval $(call BuildPackage,boxbackup-certs))
$(eval $(call BuildPackage,bbstored-config-external))
$(eval $(call BuildPackage,bbackupd))
