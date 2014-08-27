#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mgen
PKG_VERSION:=5.01b
PKG_RELEASE:=2

PKG_SOURCE:=src-$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://downloads.pf.itd.nrl.navy.mil/mgen/
PKG_MD5SUM:=1e467131446d649c8a0aebdcd8e5f2a9

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/mgen
  TITLE:=Multi-Generator
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpthread $(CXX_DEPENDS)
  URL:=http://cs.itd.nrl.navy.mil/work/mgen/
endef

define Package/mgen/Description
       The Multi-Generator (MGEN) is open source software developed by
       the Naval Research Laboratory (NRL) PROTocol Engineering Advanced
       Networking (PROTEAN) Research Group. MGEN provides the ability to
       perform IP network performance tests and measurements using UDP/IP
       traffic (TCP is currently being developed).
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/unix \
		CC="$(TARGET_CXX)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)"  \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		SYSTEM_LIBS="-ldl -lpthread" \
		-f Makefile.linux mgen
endef

define Package/mgen/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/unix/mgen $(1)/usr/bin/mgen
endef

$(eval $(call BuildPackage,mgen))
