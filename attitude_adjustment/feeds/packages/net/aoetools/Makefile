#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=aoetools
PKG_VERSION:=32
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/aoetools
PKG_MD5SUM:=62c8b5664d413019d0008f27ab5dc4d1

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/aoetools
  SECTION:=net
  CATEGORY:=Network
  TITLE:=ATA over Ethernet tools
  URL:=http://aoetools.sourceforge.net/
  DEPENDS:=+kmod-aoe +libpthread
endef

define Package/aoetools/description
	The aoetools are programs for users of the ATA over Ethernet (AoE)
	network storage protocol, a simple protocol for using storage over an
	ethernet LAN. The vblade program (storage target) exports a block
	device using AoE.
endef

CONFIGURE_ARGS +=  NPERSHELF=16

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/aoe-{discover,flush,interfaces,mkdevs,mkshelf,revalidate,sancheck,stat,version} \
		$(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/aoe{cfg,ping} $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
