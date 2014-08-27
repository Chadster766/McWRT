#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=vblade
PKG_VERSION:=20
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=@SF/aoetools/vblade
PKG_MD5SUM:=3c80e4a6bc7d66ae0c235b88cb44bd59

include $(INCLUDE_DIR)/package.mk

define Package/vblade
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Vblade AoE Target
  URL:=http://aoetools.sourceforge.net/
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/vblade
endef


define Package/vblade/description
	The vblade is the virtual EtherDrive (R) blade, a program that makes a
	seekable file available over an ethernet local area network (LAN) via
	the ATA over Ethernet (AoE) protocol.
endef

CONFIGURE_ARGS +=  NPERSHELF=16

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vblade $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vbladed $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) files/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
