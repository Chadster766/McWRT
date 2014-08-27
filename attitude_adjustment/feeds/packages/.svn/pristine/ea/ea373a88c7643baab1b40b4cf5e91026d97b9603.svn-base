#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nrpe
PKG_VERSION:=2.12
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/nagios
PKG_MD5SUM:=b2d75e2962f1e3151ef58794d60c9e97

include $(INCLUDE_DIR)/package.mk

define Package/nrpe
  SECTION:=admin
  CATEGORY:=Administration
  DEPENDS:=+libopenssl +libwrap
  TITLE:=Daemon to execute Nagios check commands on remote hosts
  URL:=http://www.nagios.org/download
endef

define Package/nrpe/description
	The NRPE addon is designed to allow you to execute Nagios plugins on
	remote Linux/Unix machines.  The main reason for doing this is to allow
	Nagios to monitor "local" resources (like CPU load, memory usage, etc.)
	on remote machines.  Since these public resources are not usually
	exposed to external machines, an agent like NRPE must be installed on
	the remote Linux/Unix machines.
endef

CONFIGURE_ARGS += \
	--with-ssl="$(STAGING_DIR)/usr" \
	--with-ssl-lib="$(STAGING_DIR)/usr/lib"

define Package/nrpe/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DATA) ./files/nrpe.cfg $(1)/etc/nrpe.cfg
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/nrpe $(1)/usr/sbin
	$(INSTALL_BIN) ./files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
endef

define Package/nrpe/conffiles
/etc/nrpe.cfg
endef

$(eval $(call RequireCommand,openssl, \
	$(PKG_NAME) requires openssl installed on the host-system. \
))

$(eval $(call BuildPackage,nrpe))

