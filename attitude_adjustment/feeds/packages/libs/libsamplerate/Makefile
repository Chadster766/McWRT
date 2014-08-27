#
# Copyright (C) 2007-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsamplerate
PKG_VERSION:=0.1.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.mega-nerd.com/SRC/
PKG_MD5SUM:=ad093e60ec44f0a60de8e29983ddbc0f

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libsamplerate
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libsndfile
  TITLE:=Sample Rate Converter
  URL:=http://www.mega-nerd.com/SRC/index.html
endef

define Package/libsamplerate/description
 Secret Rabbit Code (aka libsamplerate) is a Sample Rate
 Converter for audio.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--disable-fftw \

define Build/InstallDev
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

define Package/libsamplerate/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsamplerate.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsamplerate))
