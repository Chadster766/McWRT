#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# iconv full
ifeq ($(CONFIG_BUILD_NLS),y)
	ICONV_PREFIX:=$(STAGING_DIR)/usr/lib/libiconv-full
	ICONV_FULL:=1

	INTL_PREFIX:=$(STAGING_DIR)/usr/lib/libintl-full
	INTL_FULL:=1

# iconv stub
else
	ICONV_PREFIX:=$(STAGING_DIR)/usr/lib/libiconv-stub
	ICONV_FULL:=

	INTL_PREFIX:=$(STAGING_DIR)/usr/lib/libintl-stub
	INTL_FULL:=
endif

PKG_CONFIG_DEPENDS += CONFIG_BUILD_NLS
PKG_BUILD_DEPENDS += !BUILD_NLS:libiconv !BUILD_NLS:libintl

ICONV_DEPENDS:=+BUILD_NLS:libiconv-full
ICONV_CFLAGS:=-I$(ICONV_PREFIX)/include
ICONV_CPPFLAGS:=-I$(ICONV_PREFIX)/include
ICONV_LDFLAGS:=-L$(ICONV_PREFIX)/lib

INTL_DEPENDS:=+BUILD_NLS:libintl-full
INTL_CFLAGS:=-I$(INTL_PREFIX)/include
INTL_CPPFLAGS:=-I$(INTL_PREFIX)/include
INTL_LDFLAGS:=-L$(INTL_PREFIX)/lib

TARGET_CFLAGS += $(ICONV_CFLAGS) $(INTL_CFLAGS)
TARGET_CPPFLAGS += $(ICONV_CFLAGS) $(INTL_CPPFLAGS)
TARGET_LDFLAGS += $(ICONV_LDFLAGS) $(INTL_LDFLAGS)
