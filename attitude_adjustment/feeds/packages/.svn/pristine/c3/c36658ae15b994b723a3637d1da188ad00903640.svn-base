# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cwiid
PKG_VERSION:=0.6.00
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://abstrakraft.org/cwiid/downloads/
PKG_SOURCE_VERSION:=2100f14c612471084434b364501e3818c7f4144e

PKG_BUILD_DEPENDS:=python

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/cwiid/Default
  TITLE:=Linux Nintendo Wiimote interface
  URL:=http://abstrakraft.org/cwiid/
endef

define Package/libcwiid
$(call Package/cwiid/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
  DEPENDS+= +bluez-libs +libpthread +librt
endef

define Package/wminput
$(call Package/cwiid/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE+= (utility)
  DEPENDS+= +libcwiid
endef

define Package/python-cwiid
$(call Package/cwiid/Default)
  SUBMENU:=Python
  SECTION:=lang
  CATEGORY:=Languages
  TITLE:=Python bindings for the cwiid libs
  DEPENDS:= +python-mini +libcwiid
endef

CONFIGURE_ARGS += \
	--without-python \
	--disable-ldconfig \
	--enable-shared \
	--enable-static \

TARGET_CFLAGS += $(FPIC)
TARGET_CPPFLAGS += -I$(PKG_BUILD_DIR)/libcwiid/
TARGET_LDFLAGS += -lpthread -Wl,-rpath-link=$(STAGING_DIR)/usr/lib

define Build/Prepare
	$(call Build/Prepare/Default)
	( cd $(PKG_BUILD_DIR) ; \
		autoconf ; \
	)
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC)" \
		OFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS) -L../libcwiid/" \
		all
	$(if $(Build/Compile/PyMod),,@echo Python packaging code not found.; false)
	$(call Build/Compile/PyMod,./python/, \
		install --prefix="$(PKG_INSTALL_DIR)/usr", \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/libcwiid/*.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcwiid/*.so $(1)/usr/lib/
endef

define Package/libcwiid/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcwiid/libcwiid.so.1.0 $(1)/usr/lib/
endef

define Package/wminput/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/wminput/wminput $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/lswm/lswm $(1)/usr/bin/
endef

define PyPackage/python-cwiid/filespec
+|$(PYTHON_PKG_DIR)/cwiid.so
endef

$(eval $(call BuildPackage,libcwiid))
$(eval $(call BuildPackage,wminput))
$(eval $(call PyPackage,python-cwiid))
$(eval $(call BuildPackage,python-cwiid))
