#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tftp-hpa
PKG_VERSION:=0.48
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@KERNEL/software/network/tftp
PKG_MD5SUM:=73dd29dfa9062a174163f6a267e0b5f7

include $(INCLUDE_DIR)/package.mk

define Package/tftp-hpa/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=File Transfer
  DEPENDS:=+libreadline
  URL:=http://www.kernel.org/pub/software/network/tftp
endef

define Package/tftp-hpa
  $(call Package/tftp-hpa/Default)
  TITLE:=An enhanced version of the BSD TFTP client
endef

define Package/tftpd-hpa
  $(call Package/tftp-hpa/Default)
  TITLE:=An enhanced version of the BSD TFTP server
endef

define Build/Configure	
	$(call Build/Configure/Default, \
		--without-tcpwrappers, \
		CFLAGS="$$$$CFLAGS $$$$CPPFLAGS" \
	)
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/tftp-hpa/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tftp/tftp $(1)/usr/bin/tftp-hpa
endef

define Package/tftpd-hpa/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tftpd/tftpd $(1)/usr/sbin/tftpd-hpa
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/tftpd-hpa.init $(1)/etc/init.d/tftpd-hpa
endef

$(eval $(call BuildPackage,tftp-hpa))
$(eval $(call BuildPackage,tftpd-hpa))
