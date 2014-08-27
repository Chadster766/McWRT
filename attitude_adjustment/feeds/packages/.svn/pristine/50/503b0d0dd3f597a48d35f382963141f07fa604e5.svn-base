#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uftp
PKG_VERSION:=3.6.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar
PKG_SOURCE_URL:=https://www.tcnj.edu/~bush/downloads
PKG_MD5SUM:=7a0d41a728cd9e36ad396d7c2adeed5c

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/uftp/Default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=File Transfer
  TITLE:=Multicast UDP FTP
  URL:=https://www.tcnj.edu/~bush/uftp.html
endef

define Package/uftp/Default/description
  UFTP is an encrypted multicast file transfer program, designed to securely,
  reliably, and efficiently transfer files to multiple receivers
  simultaneously.
endef

define Package/uftp
  $(call Package/uftp/Default)
  TITLE+= server with TLS support
  VARIANT:=tls
  DEPENDS:=+libopenssl
endef

define Package/uftp/description
  $(call Package/uftp/Default/description)
  This package contains the server built with TLS support.
endef

define Package/uftp-notls
  $(call Package/uftp/Default)
  TITLE+= server without TLS support
  VARIANT:=notls
endef

define Package/uftp-notls/description
  $(call Package/uftp/Default/description)
  This package contains the server built without TLS support.
endef

define Package/uftpd
  $(call Package/uftp/Default)
  TITLE+= client daemon with TLS support
  VARIANT:=tls
  DEPENDS:=+libopenssl
endef

define Package/uftpd/description
  $(call Package/uftp/Default/description)
  This package contains the client daemon built with TLS support.
endef

define Package/uftpd-notls
  $(call Package/uftp/Default)
  TITLE+= client daemon without TLS support
  VARIANT:=notls
endef

define Package/uftpd-notls/description
  $(call Package/uftp/Default/description)
  This package contains the client daemon built without TLS support.
endef

define Package/uftpproxyd
  $(call Package/uftp/Default)
  TITLE+= proxy daemon with TLS support
  VARIANT:=tls
  DEPENDS:=+libopenssl
endef

define Package/uftpproxyd/description
  $(call Package/uftp/Default/description)
  This package contains the proxy daemon built with TLS support.
endef

define Package/uftpproxyd-notls
  $(call Package/uftp/Default)
  TITLE+= proxy daemon without TLS support
  VARIANT:=notls
endef

define Package/uftpproxyd-notls/description
  $(call Package/uftp/Default/description)
  This package contains the proxy daemon built without TLS support.
endef

define Package/uftp-keymgt
  $(call Package/uftp/Default)
  TITLE+= key management utility
  VARIANT:=tls
  DEPENDS:=+libopenssl
endef

define Package/uftp-keymgt/description
  $(call Package/uftp/Default/description)
  This package contains the key management utility.
endef

MAKE_FLAGS += \
	CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
	UNAME_S="Linux"

ifeq ($(BUILD_VARIANT),tls)
	MAKE_FLAGS += \
		OPENSSL="$(STAGING_DIR)/usr"
endif

ifeq ($(BUILD_VARIANT),notls)
	MAKE_FLAGS += \
		NO_ENCRYPTION="1"
endif

define Package/uftp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/uftp $(1)/usr/bin/
endef

define Package/uftpd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/uftpd $(1)/usr/sbin/
endef

define Package/uftpproxyd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/uftpproxyd $(1)/usr/sbin/
endef

Package/uftp-notls/install=$(Package/uftp/install)
Package/uftpd-notls/install=$(Package/uftpd/install)
Package/uftpproxyd-notls/install=$(Package/uftpproxyd/install)

define Package/uftp-keymgt/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/uftp_keymgt $(1)/usr/bin/
endef

$(eval $(call BuildPackage,uftp))
$(eval $(call BuildPackage,uftp-notls))
$(eval $(call BuildPackage,uftpd))
$(eval $(call BuildPackage,uftpd-notls))
$(eval $(call BuildPackage,uftpproxyd))
$(eval $(call BuildPackage,uftpproxyd-notls))
$(eval $(call BuildPackage,uftp-keymgt))
