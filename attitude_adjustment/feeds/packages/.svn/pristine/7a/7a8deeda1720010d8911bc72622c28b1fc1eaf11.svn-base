#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnet0
PKG_VERSION:=1.0.2a
PKG_RELEASE:=8

PKG_SOURCE:=libnet-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.packetfactory.net/libnet/dist/deprecated/
PKG_MD5SUM:=ddf53f0f484184390e8c2a1bd0853667

PKG_BUILD_DIR:=$(BUILD_DIR)/Libnet-$(PKG_VERSION)
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libnet0
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libpcap
  TITLE:=Low-level packet creation library (v1.0.x)
  URL:=http://www.packetfactory.net/libnet/
endef

ENDIANESS:=lil
ifeq ($(CONFIG_BIG_ENDIAN),y)
  ENDIANESS:=big
endif

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--with-pf_packet=yes

CONFIGURE_VARS += \
	ac_cv_libnet_endianess=$(ENDIANESS) \
	ac_libnet_have_pf_packet=yes \
	LL_INT_TYPE=sockpacket

define Build/Configure
	(cd $(PKG_BUILD_DIR); touch \
		configure.in \
		include.m4 \
		aclocal.m4 \
		Makefile.in \
		configure \
	);
	$(call Build/Configure/Default)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib/libnet-1.0.x/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/libnet-config $(1)/usr/lib/libnet-1.0.x/bin/
	$(INSTALL_DIR) $(1)/usr/lib/libnet-1.0.x/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libnet* $(1)/usr/lib/libnet-1.0.x/include/
	$(INSTALL_DIR) $(1)/usr/lib/libnet-1.0.x/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnet.{a,so*} $(1)/usr/lib/libnet-1.0.x/lib/
endef

define Package/libnet0/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnet.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnet0))
