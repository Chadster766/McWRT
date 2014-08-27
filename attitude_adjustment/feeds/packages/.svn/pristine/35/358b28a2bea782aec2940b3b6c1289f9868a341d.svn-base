#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=flashrom
PKG_VERSION:=0.9.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.flashrom.org/releases
PKG_MD5SUM:=d739a91aece12cc1e622038bb199541c

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/flashrom
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=FlashROM Utility
  URL:=http://www.flashrom.org/
# XXX: needs <sys/io.h>, only available on these arch right now
  DEPENDS:=+zlib +pciutils +dmidecode +libftdi +libusb @(arm||armeb||i386||i686||x86_64)
endef

define Package/flashrom/description
 flashrom is a utility for identifying, reading, writing, verifying
 and erasing flash chips. It's often used to flash BIOS/EFI/coreboot
 /firmware images.
endef

MAKE_FLAGS += \
	PREFIX="/usr"

define Package/flashrom/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/flashrom $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,flashrom))
