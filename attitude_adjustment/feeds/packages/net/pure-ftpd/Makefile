#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pure-ftpd
PKG_VERSION:=1.0.32
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://download.pureftpd.org/pub/pure-ftpd/releases
PKG_MD5SUM:=065211a4319e2089bd16b44c003eb0f8

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/pure-ftpd/Default
  SUBMENU:=File Transfer
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libelf
  TITLE:=Secure FTP made easy!
  URL:=http://www.pureftpd.org/project/pure-ftpd
endef

define Package/pure-ftpd
  $(call Package/pure-ftpd/Default)
  TITLE+= (no TLS)
  VARIANT:=notls
endef

define Package/pure-ftpd-tls
  $(call Package/pure-ftpd/Default)
  TITLE+= (TLS)
  VARIANT:=tls
  DEPENDS+=+libopenssl
endef

define Package/pure-ftpd/description
 Pure-FTPd is a free (BSD), secure, production-quality and standard-conformant
 FTP server. It doesn't provide useless bells and whistles, but focuses on
 efficiency and ease of use.
 It provides simple answers to common needs, plus unique  useful features for
 personal users as well as hosting providers.
endef

Package/pure-ftpd-tls/description=$(Package/pure-ftpd/description)

define Package/pure-ftpd/conffiles
/etc/config/pure-ftpd
endef

Package/pure-ftpd-tls/conffiles=$(Package/pure-ftpd/conffiles)

CONFIGURE_ARGS += \
	--with-everything \
	--with-virtualchroot \
	--without-capabilities \

ifeq ($(BUILD_VARIANT),tls)
CONFIGURE_ARGS += \
	--with-certfile=/etc/ssl/private/pure-ftpd.pem \
	--with-tls
endif

define Package/pure-ftpd/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/$(PKG_NAME).config $(1)/etc/config/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/pure-{pw,pwconvert,statsdecode} \
		$(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ptracetest $(1)/usr/sbin/
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/pure-{authd,ftpd,ftpwho,mrtginfo,quotacheck,uploadscript} \
		$(1)/usr/sbin/
endef

Package/pure-ftpd-tls/install=$(Package/pure-ftpd/install)

$(eval $(call BuildPackage,pure-ftpd))
$(eval $(call BuildPackage,pure-ftpd-tls))
