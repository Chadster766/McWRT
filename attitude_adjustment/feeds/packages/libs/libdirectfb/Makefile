#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=DirectFB
PKG_VERSION:=1.4.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.directfb.org/downloads/Core/DirectFB-1.4
PKG_MD5SUM:=e4376c6c5b8e27d735edb7f62a7a8e86

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/directfb
    TITLE:=DirectFB
    SECTION:=libs
    CATEGORY:=Libraries
    URL:=http://directfb.org
    DEPENDS:=+libpng +libjpeg +libpthread +libfreetype +libsysfs
endef

define Package/directfb/description
    DirectFB is a thin library that provides hardware graphics acceleration, input device handling and abstraction, integrated windowing system with support for translucent windows and multiple display layers, not only on top of the Linux Framebuffer Device. It is a complete hardware abstraction layer with software fallbacks for every graphics operation that is not supported by the underlying hardware.
endef

TARGET_LDFLAGS+="-Wl,-rpath-link=$(STAGING_DIR)/usr/lib"
CONFIGURE_CMD=./autogen.sh

define Feature/DirectFB
  TARGET_NAME:=drawing-backend
  TARGET_TITLE:=Drawing Backend
  NAME:=DirectFB
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--disable-osx \
		--disable-x11 \
		--enable-debug-support \
		--disable-network \
		--disable-mmx \
		--disable-sse \
		--enable-fbdev \
		--disable-sdl \
		--disable-vnc \
		--enable-sysfs \
		--with-inputdrivers=linuxinput \
		--with-gfxdrivers=none \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) \
		$(1)/usr/include \
		$(1)/usr/lib \
		$(1)/usr/lib/pkgconfig
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig \
		$(1)/usr/lib/
endef

define Package/directfb/install
	$(INSTALL_DIR) \
		$(1)/usr/bin \
		$(1)/usr/lib \
		$(1)/usr/share/directfb-1.4.2 \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBFont \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBImageProvider \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBVideoProvider \
		$(1)/usr/lib/directfb-1.4-0/wm \
		$(1)/usr/lib/directfb-1.4-0/systems \
		$(1)/usr/lib/directfb-1.4-0/inputdrivers
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/bin/* \
		$(1)/usr/bin/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/share/directfb-1.4.2/* \
		$(1)/usr/share/directfb-1.4.2/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/lib{direct,fusion}*.so* \
		$(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/interfaces/IDirectFBFont/*.so* \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBFont/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/interfaces/IDirectFBImageProvider/*.so* \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBImageProvider/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/interfaces/IDirectFBVideoProvider/*.so* \
		$(1)/usr/lib/directfb-1.4-0/interfaces/IDirectFBVideoProvider/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/wm/*.so* \
		$(1)/usr/lib/directfb-1.4-0/wm/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/systems/*.so* \
		$(1)/usr/lib/directfb-1.4-0/systems/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/directfb-1.4-0/inputdrivers/*.so* \
		$(1)/usr/lib/directfb-1.4-0/inputdrivers/
endef

$(eval $(call Feature,DirectFB))
$(eval $(call BuildPackage,directfb))
