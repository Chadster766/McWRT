#
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ftpd-topfield
PKG_VERSION:=0.7.4
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/puppy
PKG_MD5SUM:=4a29f04fd41aaa5890e2f202d936e270

include $(INCLUDE_DIR)/package.mk

define Package/ftpd-topfield
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=@USB_SUPPORT +libusb
  TITLE:=Topfield PVR access via FTP
  URL:=http://puppy.sourceforge.net/
endef

define Package/ftpd-topfield/description
FTP server to export the filesystem of a connected Topfield PVR
endef

define Build/Configure
endef

define Package/ftpd-topfield/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ftpd $(1)/usr/sbin/ftpd-topfield
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ftpd-topfield.init $(1)/etc/init.d/ftpd-topfield
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/ftpd-topfield.config $(1)/etc/config/ftpd-topfield
endef

define Package/ftpd-topfield/conffiles
/etc/config/ftpd-topfield
endef

$(eval $(call BuildPackage,ftpd-topfield))
