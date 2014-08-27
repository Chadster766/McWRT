#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=freecwmp
PKG_VERSION:=2012-09-25
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://dev.freecwmp.org/freecwmp
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=4bfbd994921dee06a7a28bed1f2dee3d527490f4
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz

PKG_FIXUP:=autoreconf

PKG_CONFIG_DEPENDS:= \
	CONFIG_FREECWMP_ACS_MULTI \
	CONFIG_FREECWMP_ACS_HDM \
	CONFIG_FREECWMP_DEBUG \
	CONFIG_FREECWMP_DEVEL_DEBUG

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/freecwmp/Default
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=CWMP client
  DEPENDS:=+libfreecwmp +libuci +libubox +libubus +libmicroxml +shflags
endef

define Package/freecwmp/description
 A free client implementation of CWMP (TR-069) protocol
endef

define Package/freecwmp-curl
  $(call Package/freecwmp/Default)
  TITLE+= (using libcurl)
  DEPENDS+= +libcurl
  VARIANT:=curl
endef

define Package/freecwmp-zstream
  $(call Package/freecwmp/Default)
  TITLE+= (using libzstream)
  DEPENDS+= +libzstream
  VARIANT:=zstream
endef

define Package/freecwmp-zstream/config
	source "$(SOURCE)/Config.in"
endef

USE_LOCAL=$(shell ls ./src/ 2>/dev/null >/dev/null && echo 1)
ifneq ($(USE_LOCAL),)
define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef
endif

TARGET_CFLAGS += \
	-D_GNU_SOURCE

TARGET_LDFLAGS += \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

CONFIGURE_ARGS += \
	--with-libfreecwmp-include-path=$(STAGING_DIR)/usr/include \
	--with-uci-include-path=$(STAGING_DIR)/usr/include \
	--with-libubox-include-path=$(STAGING_DIR)/usr/include \
	--with-libubus-include-path=$(STAGING_DIR)/usr/include

ifeq ($(BUILD_VARIANT),zstream)
CONFIGURE_ARGS += \
	--enable-http=zstream \
	--with-zstream-include-path=$(STAGING_DIR)/usr/include
endif

ifeq ($(BUILD_VARIANT),curl)
CONFIGURE_ARGS += \
	--enable-http=curl
endif

ifeq ($(CONFIG_FREECWMP_ACS_MULTI),y)
CONFIGURE_ARGS += \
	--enable-acs=multi
endif

ifeq ($(CONFIG_FREECWMP_ACS_HDM),y)
CONFIGURE_ARGS += \
	--enable-acs=hdm
endif

ifeq ($(CONFIG_FREECWMP_DEBUG),y)
CONFIGURE_ARGS += \
	--enable-debug
endif

ifeq ($(CONFIG_FREECWMP_DEVEL_DEBUG),y)
CONFIGURE_ARGS += \
	--enable-devel
endif

define Package/freecwmp-$(BUILD_VARIANT)/conffiles
/etc/config/freecwmp
/usr/share/freecwmp/defaults
endef

define Package/freecwmp-$(BUILD_VARIANT)/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/bin/freecwmpd $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/ext/openwrt/config/freecwmp $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/freecwmpd.init $(1)/etc/init.d/freecwmpd
ifeq ($(CONFIG_FREECWMP_SCRIPTS_FULL),y)
	$(INSTALL_DIR) $(1)/usr/share/freecwmp
	$(CP) $(PKG_BUILD_DIR)/ext/openwrt/scripts/defaults $(1)/usr/share/freecwmp
	$(CP) $(PKG_BUILD_DIR)/ext/openwrt/scripts/functions $(1)/usr/share/freecwmp
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ext/openwrt/scripts/freecwmp.sh $(1)/usr/sbin/freecwmp
endif
endef

$(eval $(call BuildPackage,freecwmp-curl))
$(eval $(call BuildPackage,freecwmp-zstream))
