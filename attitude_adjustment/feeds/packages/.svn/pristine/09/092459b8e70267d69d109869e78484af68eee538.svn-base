#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libgphoto2
PKG_VERSION:=2.4.12
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/gphoto
PKG_MD5SUM:=735c50ebb15cb8be61bac400ee4afb1e

PKG_FIXUP:=autoreconf
PKG_LIBTOOL_PATHS:=. libgphoto2_port
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/libgphoto2/Default
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=http://www.gphoto.org/
endef

define Package/libgphoto2
  $(call Package/libgphoto2/Default)
  DEPENDS:=+libexif +libjpeg +libpthread +libltdl $(ICONV_DEPENDS)
  TITLE:=The basic library of the gphoto2 program.
  MENU:=1
endef

define Package/libgphoto2-drivers
  $(call Package/libgphoto2/Default)
  DEPENDS:=libgphoto2 +libusb-1.0 +libgd
  TITLE:=Gphoto2 drivers for various cameras
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-rpath \
	--with-drivers=all \
	--without-included-ltdl \
	--without-libiconv-prefix \
	--without-libintl-prefix \

CONFIGURE_VARS += \
	CPPFLAGS="$$$$CPPFLAGS $(ICONV_CFLAGS)" \
	LDFLAGS="$$$$LDFLAGS $(ICONV_LDFLAGS)" \
	LIBEXIF_CFLAGS="$$$$CPPFLAGS" \
	LIBEXIF_LIBS="$$$$LDFLAGS -lexif" \
	LIBUSB_CFLAGS="$$$$CPPFLAGS" \
	LIBUSB_LIBS="$$$$LDFLAGS -lusb" \
	LIBS="-lltdl" \

# If OpenWrt is using the iconv stub, we disable iconv support
# in libgphoto2 entirely since the stub lacks some essential
# conversions like UCS-2 to UTF-8 which will let certain drivers
# fail with "Failed to create iconv converter" .
ifneq ($(ICONV_FULL),1)
  CONFIGURE_VARS += am_cv_func_iconv=no am_cv_lib_iconv=no
endif

MAKE_FLAGS += \
	LIBLTDL="" \

TARGET_CFLAGS += $(FPIC)

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/gphoto2{,-port}-config $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gphoto2 $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgphoto2{,_port}.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libgphoto2.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I$$$${prefix}/include/gphoto2,,g' $(1)/usr/bin/gphoto2{,-port}-config
	$(SED) 's,-I$$$${prefix}/include,,g' $(1)/usr/bin/gphoto2{,-port}-config
	# remove annoying recursive symlink
	rm -f $(1)/usr/include/gphoto2/gphoto2
endef

define Package/libgphoto2/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgphoto2{,_port}.so.* $(1)/usr/lib/
endef

define Package/libgphoto2-drivers/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgphoto2{,_port} $(1)/usr/lib/
	rm -rf $(1)/usr/lib/libgphoto2{,_port}/*/*.{,l}a
	rm -f $(1)/usr/lib/libgphoto2_port/*/usb.so
endef

$(eval $(call BuildPackage,libgphoto2))
$(eval $(call BuildPackage,libgphoto2-drivers))
