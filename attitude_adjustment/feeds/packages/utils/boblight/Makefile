#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=boblight
PKG_REV:=412
PKG_VERSION:=$(PKG_REV)
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://boblight.googlecode.com/svn/trunk
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_PROTO:=svn
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/boblight/Default
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Tools for driving lights connected to an external controller
  URL:=http://code.google.com/p/boblight/
  DEPENDS:=+librt $(CXX_DEPENDS)
endef

define Package/boblight/Default/description
  Its main purpose is to create light effects from an external input, such as a
  video stream (desktop capture, video player, tv card), an audio stream (jack,
  alsa), or user input (lirc, http). Currently it only handles video input by
  desktop capture with xlib, video capture from v4l/v4l2 devices and user input
  from the commandline with boblight-constant.

  Boblight uses a client/server model, where clients are responsible for
  translating an external input to light data, and boblightd is responsible for
  translating the light data into commands for external light controllers.
endef

define Package/boblight-client
  $(call Package/boblight/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (constant color client)
  DEPENDS+= +libboblight
endef

define Package/boblight-client/description
  $(call Package/boblight/Default/description)

  This package contains the boblight constant color client.
endef

define Package/boblight-daemon
  $(call Package/boblight/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (daemon)
  DEPENDS+= +libboblight
endef

define Package/boblight-daemon/conffiles
/etc/boblight.conf
endef

define Package/boblight-daemon/description
  $(call Package/boblight/Default/description)

  This package contains the boblight daemon.
endef

define Package/libboblight
  $(call Package/boblight/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
endef

define Package/libboblight/description
  $(call Package/boblight/Default/description)

  This package contains the libboblight shared library.
endef

CONFIGURE_ARGS += \
	--without-portaudio \
	--without-x11


define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/libboblight
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libboblight $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libboblight.{a,so*} $(1)/usr/lib/
endef

define Package/boblight-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/boblight-constant $(1)/usr/bin/
endef

define Package/boblight-daemon/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/boblight.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/boblight.init $(1)/etc/init.d/boblight
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/boblightd $(1)/usr/bin/
endef

define Package/libboblight/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libboblight.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,boblight-client))
$(eval $(call BuildPackage,boblight-daemon))
$(eval $(call BuildPackage,libboblight))
