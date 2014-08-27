#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=flite
PKG_VERSION:=1.3-release
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.speech.cs.cmu.edu/flite/packed/flite-1.3/
PKG_MD5SUM:=ae0aca1cb7b4801f4372f3a75a9e52b5

PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/flite
  SECTION:=sound
  CATEGORY:=Sound
  DEPENDS:=@!GCC_VERSION_3_4_6
  TITLE:=Text-to-speech for embedded systems
  URL:=http://www.speech.cs.cmu.edu/flite/index.html
endef

define Package/flite/description
Festival Lite is a stripped down version of Festival,
	the well-developed text-to-speech program written in C++.
	This program is written in C to keep memory usage low.
endef

TARGET_LDFLAGS += -lm

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--with-audio="oss" \
		--with-vox="cmu_us_kal16" \
		--prefix="$(PKG_INSTALL_DIR)/usr" \
		--exec-prefix="$(PKG_INSTALL_DIR)/usr" \
		--bindir="$(PKG_INSTALL_DIR)/usr/bin" \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/flite
	$(CP) $(PKG_INSTALL_DIR)/usr/include/flite/{cst,flite}*.h $(1)/usr/include/flite/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libflite*.so.* $(1)/usr/lib/
endef

define Package/flite/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/flite $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libflite*.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,flite))
