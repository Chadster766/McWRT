#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipv6calc
PKG_VERSION:=0.93.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc
PKG_MD5SUM:=9a5b7fdd1a90cd621dc43623976d912a

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ipv6calc/Default
  SECTION:=ipv6
  CATEGORY:=IPv6
  URL:=http://www.deepspace6.net/projects/ipv6calc.html
endef

define Package/ipv6calc/Default/description
  ipv6calc is a small utility written in programming languate C to manipulate
  (not only) IPv6 addresses. Intentions were convering a given IPv6 address
  into compressed format, convering a given IPv6 address into the same format
  like shown in /proc/net/if_inet6 and (because it was not difficulty)
  migrating the Perl program ip6_int into. Now only one utiltity is needed to
  do a lot.
endef

define Package/ipv6calc
  $(call Package/ipv6calc/Default)
  TITLE:=IPv6 addresses calculations (full)
  VARIANT:=full
endef

define Package/ipv6calc/description
  $(call Package/ipv6calc/Default/description)
  This package contains an ipv6calc with the internal databases enabled.
endef

define Package/ipv6calc-mini
  $(call Package/ipv6calc/Default)
  TITLE:=IPv6 addresses calculations (mini)
  VARIANT:=mini
endef

define Package/ipv6calc-mini/description
  $(call Package/ipv6calc/Default/description)
  This package contains an ipv6calc with the internal databases disabled.
endef

define Package/ipv6log
  $(call Package/ipv6calc/Default)
  TITLE:=Webserver log helper programs (full)
  VARIANT:=full
endef

define Package/ipv6log/description
  $(call Package/ipv6calc/Default/description)
  This package contains webserver log anonymization, conversion, and statistics
  helper programs with the internal databases enabled.
endef

define Package/ipv6log-mini
  $(call Package/ipv6calc/Default)
  TITLE:=Webserver log helper programs (mini)
  VARIANT:=mini
endef

define Package/ipv6log-mini/description
  $(call Package/ipv6calc/Default/description)
  This package contains webserver log anonymization, conversion, and statistics
  helper programs with the internal databases disabled.
endef

ifeq ($(BUILD_VARIANT),full)
	CONFIGURE_ARGS += \
		--enable-db-ieee \
		--enable-db-ipv4 \
		--enable-db-ipv6
endif

ifeq ($(BUILD_VARIANT),mini)
	CONFIGURE_ARGS += \
		--disable-db-ieee \
		--disable-db-ipv4 \
		--disable-db-ipv6
endif

MAKE_FLAGS += \
	CC="$(TARGET_CC)" \
	COPTS="$(TARGET_CFLAGS) -I$(PKG_BUILD_DIR)"

define Package/ipv6calc/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ipv6calc $(1)/usr/bin/
endef

Package/ipv6calc-mini/install = $(Package/ipv6calc/install)

define Package/ipv6log/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ipv6log* $(1)/usr/bin/
endef

Package/ipv6log-mini/install = $(Package/ipv6log/install)

$(eval $(call BuildPackage,ipv6calc))
$(eval $(call BuildPackage,ipv6calc-mini))
$(eval $(call BuildPackage,ipv6log))
$(eval $(call BuildPackage,ipv6log-mini))
