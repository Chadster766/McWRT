#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dmalloc
PKG_VERSION:=5.5.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://dmalloc.com/releases/
PKG_MD5SUM:=f92e5606c23a8092f3d5694e8d1c932e

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/dmalloc/Default
  SECTION:=devel
  CATEGORY:=Development
  TITLE:=Dmalloc
  URL:=http://www.dmalloc.com/
endef

define Package/libdmalloc
  $(call Package/dmalloc/Default)
  TITLE+= (library)
  DEPENDS:=@USE_EGLIBC
endef

define Package/dmalloc-utils
  $(call Package/dmalloc/Default)
  DEPENDS:=+libdmalloc
  TITLE+= (utilities)
endef

define Package/dmalloc/description
The debug memory allocation or dmalloc library has been designed as a drop in
replacement for the system's malloc, realloc, calloc, free and other memory
management routines while providing powerful debugging facilities configurable
at runtime. These facilities include such things as memory-leak tracking,
fence-post write detection, file/line number reporting, and general logging of
statistics.
endef

CONFIGURE_ARGS += \
	--disable-cxx \
	--with-pagesize=12

TARGET_CFLAGS += $(FPIC)

MAKE_INSTALL_FLAGS += installsl

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/include/dmalloc.h $(1)/usr/include/

	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libdmalloc.{a,so*} $(1)/usr/lib/
endef

define Package/libdmalloc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/usr/lib/libdmalloc.so* $(1)/usr/lib/
endef

define Package/dmalloc-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/dmalloc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libdmalloc))
$(eval $(call BuildPackage,dmalloc-utils))
