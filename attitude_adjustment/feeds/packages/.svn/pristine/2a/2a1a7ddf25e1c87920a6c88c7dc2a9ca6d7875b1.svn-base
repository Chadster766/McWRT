#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=faifa
PKG_VERSION:=2012-09-13
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://github.com/ffainelli/faifa.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=2842afc90bd9ca08ed2a6c83c04f4015982e6007
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/faifa/Default
  TITLE:=configure HomePlug 1.0/AV PLC devices
  URL:=http://open-plc.org/
endef

define Package/libfaifa
  $(call Package/faifa/Default)
  TITLE+= (library)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libpcap +libpthread
endef

define Package/libfaifa/description
  Faifa companion library.
endef

define Package/faifa
  $(call Package/faifa/Default)
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread +libpcap +libfaifa
  TITLE+= (command line)
endef

define Package/faifa/description
  Faifa can configure any Intellon-based Power Line Communication device using
  Intellon INT5000 and INT6000 series chips (6000 and 6300 chips). It supports all
  Intellon-specific management and control frames as well as standard management
  frames.
endef

define Package/hpavcfg
  $(call Package/faifa/Default)
  SECTION:=net
  CATEGORY:=Network
  TITLE:=HomePlug AV lightweight configurator
endef

define Package/hpavcfg/description
   Lightweight configuration tool, only capable of setting a local device key (NMK)
endef

TARGET_CFLAGS += $(FPIC)

MAKE_FLAGS += \
	OS=LINUX \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)"

define Build/Configure
	(cd $(PKG_BUILD_DIR); ./autogen.sh)
	$(call Build/Configure/Default)
endef

define Package/faifa/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/faifa $(1)/usr/bin/
endef

define Package/libfaifa/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libfaifa.so* $(1)/usr/lib
endef

define Package/hpavcfg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/hpav_cfg $(1)/usr/bin
endef

$(eval $(call BuildPackage,faifa))
$(eval $(call BuildPackage,libfaifa))
$(eval $(call BuildPackage,hpavcfg))
