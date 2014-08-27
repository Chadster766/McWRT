#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=GraphicsMagick
PKG_VERSION:=1.3.8
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/graphicsmagick
PKG_MD5SUM:=c0428886ea07736b0f1872d256ab7ebd

include $(INCLUDE_DIR)/package.mk

export PATH:=$(PKG_BUILD_DIR)/bin:$(TARGET_PATH)

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

define Package/GraphicsMagick/Default
  SECTION:=multimedia
  CATEGORY:=Multimedia
  TITLE:=GraphicsMagick
  URL:=http://www.graphicsmagick.org/
  DEPENDS:=+libbz2 +libfreetype +libltdl +libpthread +libstdcpp +zlib
endef

define Package/GraphicsMagick
  $(call Package/GraphicsMagick/Default)
endef

define Package/GraphicsMagick/description
  An image manipulation library.
endef

define Package/GraphicsMagick-jpeg
  $(call Package/GraphicsMagick/Default)
  TITLE+= (jpeg)
  DEPENDS:=+GraphicsMagick +libjpeg +zlib
endef

define Package/GraphicsMagick-jpeg/description
  GraphicsMagick JPEG module
endef

CONFIGURE_ARGS += \
	  --with-modules \
	  --enable-shared \
	  --enable-static \
	  --without-xml \
	  --without-x \

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/GraphicsMagick/magick
	$(INSTALL_DIR) $(1)/usr/include/GraphicsMagick/wand
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/* \
		$(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/

	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig/
	$(INSTALL_DATA) \
		$(PKG_INSTALL_DIR)/usr/lib/pkgconfig/* \
		$(1)/usr/lib/pkgconfig/
endef

define Package/GraphicsMagick/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/*.so* \
		$(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/share/GraphicsMagick-$(PKG_VERSION)
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/share/GraphicsMagick-$(PKG_VERSION)/* \
		$(1)/usr/share/GraphicsMagick-$(PKG_VERSION)
endef

define BuildPlugin
  define Package/GraphicsMagick-$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib/GraphicsMagick-$(PKG_VERSION)/modules-Q8/coders
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/GraphicsMagick-$(PKG_VERSION)/modules-Q8/coders/$(1).{so,la} \
		$$(1)/usr/lib/GraphicsMagick-$(PKG_VERSION)/modules-Q8/coders
  endef

  $$(eval $$(call BuildPackage,GraphicsMagick-$(1)))
endef

$(eval $(call BuildPackage,GraphicsMagick))
$(eval $(call BuildPlugin,jpeg))
