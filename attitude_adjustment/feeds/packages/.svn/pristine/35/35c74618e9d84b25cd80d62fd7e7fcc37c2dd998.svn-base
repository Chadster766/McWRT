#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libdvdread
PKG_VERSION:=0.9.7
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.dtek.chalmers.se/~dvd/dist
PKG_MD5SUM:=078788c9241ae16763529e1235502337

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libdvdread
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=simple foundation for reading DVD video disks.
  URL:=http://www.dtek.chalmers.se/~dvd/downloads.shtml
endef

define Package/libdvdread/description
	libdvdread provides a simple foundation for reading DVD video disks.
	It provides the functionality that is required to access many DVDs.
	It parses IFO files, reads NAV-blocks, and performs CSS
	authentication and descrambling.
endef

TARGET_CFLAGS += \
	-ffunction-sections -fdata-sections

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static
#	--with-build-cc="$(HOSTCC)" 

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS) -I$(BUILD_DIR)/linux/include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/dvdread
	$(CP)	$(PKG_INSTALL_DIR)/usr/include/dvdread/*.h \
		$(1)/usr/include/dvdread
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)	$(PKG_INSTALL_DIR)/usr/lib/libdvdread.{a,so*} \
		$(1)/usr/lib/
endef

define Package/libdvdread/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdvdread.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libdvdread))
