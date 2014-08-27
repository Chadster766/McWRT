#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gnuplot
PKG_VERSION:=4.6.0
PKG_RELEASE:=2

PKG_INSTALL:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/gnuplot
PKG_MD5SUM:=8e6e92b4596ea0eb75e16a57fc79efec

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

TARGET_LDFLAGS+= \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib

CONFIGURE_VARS += \
	CXXFLAGS="$$$$CXXFLAGS -fno-rtti" \
	LIBS="-lpthread"

define Package/gnuplot
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libncurses $(CXX_DEPENDS) +zlib +libpng +libfreetype +libgd \
	$(INTL_DEPENDS) $(ICONV_DEPENDS) +libpthread +libreadline
  TITLE:=A portable command-line driven graphing utility
  URL:=http://sourceforge.net/projects/gnuplot/
endef

define Package/gnuplot/description
  Gnuplot is a portable command-line driven graphing utility for linux, OS/2,
  MS Windows, OSX, VMS, and many other platforms.
endef

CONFIGURE_ARGS += \
	--without-x \
	--disable-mouse \
	--without-tutorial \
	--without-row-help \
	--without-lisp-files \
	--disable-wxwidgets \
	--without-lua \
	--without-cairo

define Package/gnuplot/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gnuplot $(1)/usr/bin/
endef

$(eval $(call BuildPackage,gnuplot))
