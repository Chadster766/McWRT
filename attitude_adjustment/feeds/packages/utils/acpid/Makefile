#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=acpid
PKG_VERSION:=2.0.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://tedfelix.com/linux/
PKG_MD5SUM:=d5dd88bcfaa4a0bf51905e95115da6df

include $(INCLUDE_DIR)/package.mk

define Package/acpid
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=The ACPI Daemon (acpid) With Netlink Support
  URL:=http://tedfelix.com/linux/acpid-netlink.html
  DEPENDS:=+kmod-input-evdev +kmod-acpi-button
endef

define Package/acpid/description
  The ACPI Daemon (acpid) With Netlink Support
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		LD="$(TARGET_CC)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		all 
endef

define Package/acpid/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/acpid $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/acpi_listen $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/acpi/events
	$(INSTALL_CONF) ./files/default $(1)/etc/acpi/events/default
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_CONF) ./files/acpid.init $(1)/etc/init.d/acpid
	chmod 0750 $(1)/etc/init.d/acpid
	$(INSTALL_DIR) $(1)/etc/hotplug.d/input/
	$(INSTALL_CONF) ./files/acpid.hotplug $(1)/etc/hotplug.d/input/
endef

define Package/acpid/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	echo "waiting for input devices to come up"
	/etc/init.d/acpid enable
	sleep 5
	/etc/init.d/acpid start
	echo "please try the power button"
	exit 0
}
endef

$(eval $(call BuildPackage,acpid))
