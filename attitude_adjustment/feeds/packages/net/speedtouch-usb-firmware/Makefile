#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
#

include $(TOPDIR)/rules.mk

PKG_NAME:=speedtouch-usb-firmware
PKG_VERSION:=3
PKG_RELEASE:=012

PKG_BUILD_DIR:=$(BUILD_DIR)/speedtouch-usb-firmware-$(PKG_VERSION)
PKG_SOURCE:=SpeedTouch330_firmware_$(PKG_VERSION)$(PKG_RELEASE).zip
PKG_SOURCE_URL:=http://www.speedtouch.com/download/drivers/USB/
PKG_MD5SUM:=2551ce46ef785642f2c6768511f70ff3

include $(INCLUDE_DIR)/package.mk

define Package/speedtouch-usb-firmware
  SUBMENU:=firmware
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SpeedTouch 330 ADSL USB firmware
  DEPENDS:=+kmod-usb-atm +kmod-usb-atm-speedtouch
  PKGARCH:=all
endef

define Package/speedtouch-usb-firmware/description
	This package add the firmware for SpeedTouch330 ADSL USB modem.
endef

define Build/Configure
endef

define Build/Compile
	mv $(PKG_BUILD_DIR)/../KQD6_$(PKG_VERSION).$(PKG_RELEASE) $(PKG_BUILD_DIR)/
	mv $(PKG_BUILD_DIR)/../ZZZL_$(PKG_VERSION).$(PKG_RELEASE) $(PKG_BUILD_DIR)/
	# next lines from Gentoo ebuild
	# Extract the "stage 1" portion of the firmware
	dd if=$(PKG_BUILD_DIR)/KQD6_$(PKG_VERSION).$(PKG_RELEASE) \
		of=$(PKG_BUILD_DIR)/speedtch-1.bin.2 ibs=1 obs=991 count=991 skip=36
	dd if=$(PKG_BUILD_DIR)/ZZZL_$(PKG_VERSION).$(PKG_RELEASE) \
		of=$(PKG_BUILD_DIR)/speedtch-1.bin.4 ibs=1 obs=935 count=935 skip=32
	# Extract the "stage 2" portion of the firmware
	dd if=$(PKG_BUILD_DIR)/KQD6_$(PKG_VERSION).$(PKG_RELEASE) \
		of=$(PKG_BUILD_DIR)/speedtch-2.bin.2 ibs=1 obs=762650 count=762650 skip=1027
	dd if=$(PKG_BUILD_DIR)/ZZZL_$(PKG_VERSION).$(PKG_RELEASE) \
		of=$(PKG_BUILD_DIR)/speedtch-2.bin.4 ibs=1 obs=775545 count=775545 skip=967
endef

define Package/speedtouch-usb-firmware/install
	$(INSTALL_DIR) -d $(1)/lib/firmware
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/speedtch-1.bin.2 $(1)/lib/firmware
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/speedtch-1.bin.4 $(1)/lib/firmware
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/speedtch-2.bin.2 $(1)/lib/firmware
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/speedtch-2.bin.4 $(1)/lib/firmware
endef

define Package/speedtouch-usb-firmware/postinst
#!/bin/sh
# Symlinks for other revisions of the modem
cd "$${IPKG_INSTROOT}/lib/firmware"
for n in 1 2 ; do
	for rev in 0 1 ; do
		ln -sfn speedtch-$${n}.bin.2 speedtch-$${n}.bin.$${rev}
	done
	# Seems like a reasonable guess, for revision 3
	ln -sfn speedtch-$${stub}$${n}.bin.4 speedtch-$${n}.bin.3
done
endef

$(eval $(call BuildPackage,speedtouch-usb-firmware))
