# 
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.

include $(TOPDIR)/rules.mk

PKG_NAME:=madwimax
PKG_VERSION:=0.1.1
PKG_RELEASE:=3

PKG_SOURCE:=madwimax-0.1.1.tar.gz
PKG_SOURCE_URL:=http://madwimax.googlecode.com/files/
PKG_MD5SUM:=4ebd2d74e887e6f9d6f23067a4ad8272

include $(INCLUDE_DIR)/package.mk

define Package/madwimax
  SECTION:=net
  CATEGORY:=Network
  TITLE:=driver for  WiMAX devices based on Samsung CMC-730 chip
  URL:=http://code.google.com/p/madwimax/
  DEPENDS:=+libusb-1.0 +kmod-tun +libpthread
endef

define Package/madwimax/description
madWiMAX is a reverse-engineered Linux driver for mobile WiMAX (802.16e) devices based on Samsung CMC-730 chip. These devices are currently supported:

Samsung SWC-U200
Samsung SWC-E100
Samsung SWM-S10R (built in Samsung NC-10 netbook)
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += --without-man-pages

define Package/madwimax/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/madwimax $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/madwimax
	$(INSTALL_DIR) $(1)/etc/hotplug.d/usb
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/20-madwimax $(1)/etc/hotplug.d/usb/
	$(INSTALL_BIN) ./files/event.sh $(1)/etc/madwimax/
	$(INSTALL_BIN) ./files/madwimax.init $(1)/etc/init.d/madwimax
endef

$(eval $(call BuildPackage,madwimax))
