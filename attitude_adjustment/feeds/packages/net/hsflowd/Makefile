# 
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=hsflowd
PKG_RELEASE:=1
PKG_SOURCE_URL:=@SF/host-sflow
PKG_VERSION:=1.20
PKG_MD5SUM:=0bfc0b05fbab27a80557c5efb5e66693

PKG_SOURCE:=hsflowd-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/hsflowd
  SECTION:=net
  CATEGORY:=Base system
  DEPENDS:=+libpthread
  TITLE:=Host sFlow export agent
  URL:=http://host-sflow.sourceforge.net/
endef

define Package/sflowovsd
  SECTION:=net
  CATEGORY:=Base system
  DEPENDS:=+hsflowd
  TITLE:=Host sFlow Open vSwitch agent
  URL:=http://host-sflow.sourceforge.net/
endef


define Package/hsflowd/description
 The Host sFlow agent exports server performance metrics using the sFlow
 protocol. The agent is intended to provide scalable, multi-vendor,
 multi-OS performance monitoring with minimal impact on the systems being
 monitored.
endef

define Package/sflowovsd/description
 The Host sFlow agent can automatically configure the sFlow settings in
 the Open vSwitch.
endef

MAKE_VARS += \
	LIBVIRT=no \
	XEN_DDK=no \
	DEBIAN=no \
	PLATFORM=Linux \
	OPT="$(TARGET_CFLAGS) -std=c99"

define Package/hsflowd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/Linux/hsflowd $(1)/usr/sbin
	$(INSTALL_BIN) ./files/hsflowd.init $(1)/etc/init.d/hsflowd
	$(INSTALL_CONF) ./files/hsflowd.config $(1)/etc/config/hsflowd

endef

define Package/sflowovsd/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/Linux/sflowovsd $(1)/usr/sbin
	$(INSTALL_BIN) ./files/sflowovsd.init $(1)/etc/init.d/sflowovsd
endef

$(eval $(call BuildPackage,hsflowd))
$(eval $(call BuildPackage,sflowovsd))

