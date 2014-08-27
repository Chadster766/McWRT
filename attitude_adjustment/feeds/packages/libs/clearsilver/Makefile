#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=clearsilver
PKG_VERSION:=0.10.5
PKG_RELEASE:=5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.clearsilver.net/downloads/
PKG_MD5SUM:=b8c0c7fbe0ef5e06e0c935f134304d44

PKG_CONFIG_DEPENDS:= \
	CONFIG_CLEARSILVER_ENABLE_COMPRESSION \
	CONFIG_CLEARSILVER_ENABLE_REMOTE_DEBUGGER \
	CONFIG_CLEARSILVER_ENABLE_GETTEXT

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1
PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/clearsilver
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=ClearSilver template system
  URL:=http://www.clearsilver.net/
  MAINTAINER:=Raphaël HUCK <rhk@cksum.org>
  DEPENDS:=+CLEARSILVER_ENABLE_COMPRESSION:zlib +CLEARSILVER_ENABLE_GETTEXT:libintl
endef

define Package/clearsilver/config
  menu "Configuration"
  depends on PACKAGE_clearsilver
  source "$(SOURCE)/Config.in"
  endmenu
endef

define Package/clearsilver/description
Clearsilver is a fast, powerful, and language-neutral HTML template system. In
both static content sites and dynamic HTML applications, it provides a separation
between presentation code and application logic which makes working with your
project easier.
endef

CONFIGURE_ARGS+= \
	--disable-wdb \
	--disable-apache \
	--disable-python \
	--disable-perl \
	--disable-ruby \
	--disable-java \
	--disable-csharp \
	--prefix=/usr

ifeq ($(SDK)$(CONFIG_CLEARSILVER_ENABLE_COMPRESSION),y)
	CONFIGURE_ARGS+= \
		--enable-compression
else
	CONFIGURE_ARGS+= \
		--disable-compression
endif

ifeq ($(SDK)$(CONFIG_CLEARSILVER_ENABLE_REMOTE_DEBUGGER),y)
	CONFIGURE_ARGS+= \
		--enable-remote-debugger
else
	CONFIGURE_ARGS+= \
		--disable-remote-debugger
endif

ifeq ($(SDK)$(CONFIG_CLEARSILVER_ENABLE_GETTEXT),y)
	CONFIGURE_ARGS+= \
		--enable-gettext
else
	CONFIGURE_ARGS+= \
		--disable-gettext
endif

TARGET_CFLAGS+=$(FPIC)

MAKE_FLAGS+= \
	$(TARGET_CONFIGURE_OPTS) \
	AR="$(AR) cr" \
	LD="$(TARGET_CC) -o"

define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/clearsilver/install
	$(INSTALL_DIR) $(1)/usr/lib
endef

$(eval $(call BuildPackage,clearsilver))
