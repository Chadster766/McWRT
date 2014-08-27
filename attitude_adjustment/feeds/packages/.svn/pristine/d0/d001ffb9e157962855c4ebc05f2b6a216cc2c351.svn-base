#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=miniupnpc
PKG_VERSION:=1.8
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://miniupnp.free.fr/files
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_MD5SUM:=065bf20a20ebe605c675b7a5aaef340a

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/miniupnpc/Default
  TITLE:=Lightweight UPnP
  URL:=http://miniupnp.tuxfamily.org/
endef

define Package/miniupnpc
  $(call Package/miniupnpc/Default)
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE+= client
  DEPENDS:=+libminiupnpc
endef

define Package/libminiupnpc
  $(call Package/miniupnpc/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= library
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
	OS="Linux" \
	PREFIX="$(PKG_INSTALL_DIR)"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/miniupnpc
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/miniupnpc/*.h \
		$(1)/usr/include/miniupnpc/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.{so*,a} \
		$(1)/usr/lib/
endef

define Package/miniupnpc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/upnpc $(1)/usr/bin/
endef

define Package/libminiupnpc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/lib/libminiupnpc.so.* \
		$(1)/usr/lib/
endef

$(eval $(call BuildPackage,miniupnpc))
$(eval $(call BuildPackage,libminiupnpc))
