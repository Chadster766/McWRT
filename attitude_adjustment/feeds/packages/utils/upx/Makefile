#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=upx
PKG_VERSION:=3.08
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.bz2
PKG_SOURCE_URL:=http://upx.sourceforge.net/download \
	http://upx.sourceforge.net/download/00-OLD-VERSIONS
PKG_MD5SUM:=54c76fa52cad578ff23ef98aee91e3f5

include $(INCLUDE_DIR)/package.mk

define Package/upx
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libucl +libstdcpp +zlib
  TITLE:=The Ultimate Packer for eXecutables
  URL:=http://upx.sourceforge.net/
endef

define Package/upx/description
UPX is a free, portable, extendable, high-performance executable packer for
several different executable formats. It achieves an excellent compression ratio
and offers very fast decompression. Your executables suffer no memory overhead
or other drawbacks for most of the formats supported, because of in-place
decompression.
endef

MAKE_PATH := src
MAKE_VARS += \
	exeext=""

TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components 1 $(TAR_OPTIONS)

TARGET_CPPFLAGS += \
	-DUCL_NO_ASM \

define Package/upx/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/upx $(1)/usr/bin/
endef

$(eval $(call BuildPackage,upx))
