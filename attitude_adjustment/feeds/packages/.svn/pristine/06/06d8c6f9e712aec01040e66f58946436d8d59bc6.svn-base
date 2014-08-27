#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=librpcsecgss
PKG_VERSION:=0.14
PKG_RELEASE:=2

PKG_SOURCE:=librpcsecgss-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.citi.umich.edu/projects/nfsv4/linux/librpcsecgss/
PKG_MD5SUM:=0d4cdee46a98731b1b71e30504589281

PKG_BUILD_DEPENDS:=libgssapi
PKG_BUILD_PARALLEL:=1
PKG_INSTALL:=1
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/librpcsecgss
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=required libraries for the nfs-utils
  URL:=http://www.citi.umich.edu/projects/nfsv4/linux
endef

define Package/librpcsecgss/description
 Support libraries - for newer nfs mount command (nfs4)
endef

TARGET_CFLAGS += -I$(PKG_BUILD_DIR)/lib -I$(STAGING_DIR)/usr/include/gssglue/ $(FPIC)

MAKE_FLAGS += \
	OPT="$(TARGET_CFLAGS)" \
	INSTALLSUID="install -m 4755" \
	DESTDIR="$(PKG_INSTALL_DIR)" \
        GSSAPI_LIBS="usr/lib/libgssapi.a"

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/rpcsecgss/rpc
	$(CP) $(PKG_INSTALL_DIR)/usr/include/rpcsecgss/rpc/*.h $(1)/usr/include/rpcsecgss/rpc/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/librpcsecgss.{a,so*} $(1)/usr/lib
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/librpcsecgss.pc $(1)/usr/lib/pkgconfig
endef

$(eval $(call BuildPackage,librpcsecgss))
