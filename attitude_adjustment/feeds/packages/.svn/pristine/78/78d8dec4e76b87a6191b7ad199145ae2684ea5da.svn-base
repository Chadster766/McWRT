
include $(TOPDIR)/rules.mk

PKG_NAME:=puppy
PKG_VERSION:=1.14
PKG_RELEASE:=1

PKG_SOURCE:=puppy_$(PKG_VERSION)_src.tar.bz2
PKG_SOURCE_URL:=@SF/puppy
PKG_MD5SUM:=a7a53da5b8ba4fdb15e2fc8e1531b82c

PKG_BUILD_DIR:=$(BUILD_DIR)/puppy_$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/puppy
  SECTION:=multimedia
  CATEGORY:=Multimedia
  DEPENDS:=@USB_SUPPORT
  TITLE:=Topfield PVR file transfer client
  URL:=http://puppy.sourceforge.net/
endef

define Package/puppy/description
Client to access Topfield PVRs via the USB port
endef

define Build/Configure
endef

define Package/puppy/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/puppy $(1)/usr/bin/
endef

$(eval $(call BuildPackage,puppy))
