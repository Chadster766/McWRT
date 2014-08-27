#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=sierra-pcmcia
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://www.sierrawireless.com/resources/faq/PC_Card/AirCard850_860/
PKG_SOURCE:=Aircard_Linux.tar
PKG_MD5SUM:=9562b626d958ea02d737feb6a85f0920

include $(INCLUDE_DIR)/package.mk

define KernelPackage/pcmcia-serial-sierra
  SUBMENU:=Other modules
  TITLE:=Sierra AirCard PCMCIA support
  DEPENDS:=kmod-pcmcia-serial
  URL:=http://www.sierrawireless.com/faq/ShowFAQ.aspx?ID=118
endef

define KernelPackage/pcmcia-serial-sierra/description
 Support files for the Sierra AirCard 555, 7xx, and 8x0
endef

define Build/Compile
endef

define Package/kmod-pcmcia-serial-sierra/install
	$(INSTALL_DIR) $(1)/lib/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/../Aircard_Linux/*.cis $(1)/lib/firmware/
endef

$(eval $(call KernelPackage,pcmcia-serial-sierra))
