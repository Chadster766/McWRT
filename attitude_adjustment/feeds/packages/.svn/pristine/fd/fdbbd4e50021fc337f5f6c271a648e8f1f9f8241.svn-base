#
# Copyright (C) 2011,2012 OpenWrt.org
# Copyright (C) 2010 Jo-Philipp Wich <xm@subsignal.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mosquitto
PKG_VERSION:=0.15
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mosquitto.org/files/source/
PKG_MD5SUM:=7ae0ac38f1f379578ab5530e5dc7269e

include $(INCLUDE_DIR)/package.mk

define Package/mosquitto/default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=mosquitto - an MQTT message broker
  URL:=http://www.mosquitto.org/
endef

define Package/mosquitto
    $(Package/mosquitto/default)
endef

define Package/mosquitto/description
 mosquitto is a message broker that supports v3.1 of the MQ Telemetry
Transport protocol. MQTT provides a lightweight method for
messaging using a publish/subscribe model.
endef

define Package/mosquitto-client
    $(Package/mosquitto/default)
    TITLE:= mosquitto - client tools
    DEPENDS:= +libmosquitto
endef

define Package/mosquitto-client/description
 Command line client tools for publishing messages to MQTT servers
and subscribing to topics.
endef

define Package/libmosquitto
    $(Package/mosquitto/default)
    SECTION:=libs
    CATEGORY:=Libraries
    TITLE:= mosquitto - client library
endef

define Package/libmosquitto/description
 Library required for mosquitto's command line client tools, also for
use by any third party software that wants to communicate with a
mosquitto server.

Should be useable for communicating with any MQTT v3.1 compatible
server, such as IBM's RSMB, in addition to Mosquitto
endef


define Package/mosquitto/conffiles
/etc/mosquitto/mosquitto.conf
endef

define Package/mosquitto/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/mosquitto $(1)/usr/sbin/mosquitto
	$(INSTALL_DIR) $(1)/etc/mosquitto
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/mosquitto.conf $(1)/etc/mosquitto/mosquitto.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mosquitto.init $(1)/etc/init.d/mosquitto
endef

define Package/mosquitto-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/client/mosquitto_pub $(1)/usr/bin/mosquitto_pub
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/client/mosquitto_sub $(1)/usr/bin/mosquitto_sub
endef

# This installs files into ./staging_dir/. so that you can cross compile from the host
define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/lib/mosquitto.h $(1)/usr/include
	$(INSTALL_DIR) $(1)/usr/lib
	# This should just get symlinked, but I can't work out the magic syntax :(
	$(CP) $(PKG_BUILD_DIR)/lib/libmosquitto.so.0 $(1)/usr/lib/libmosquitto.so
endef

# This installs files on the target.  Compare with Build/InstallDev
define Package/libmosquitto/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lib/libmosquitto.so.0 $(1)/usr/lib/libmosquitto.so.0
endef
$(eval $(call BuildPackage,mosquitto))
$(eval $(call BuildPackage,libmosquitto))
$(eval $(call BuildPackage,mosquitto-client))
