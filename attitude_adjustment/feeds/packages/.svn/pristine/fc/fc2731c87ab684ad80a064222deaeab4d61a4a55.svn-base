#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=howl
PKG_VERSION:=1.0.0
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.porchdogsoft.com/download/
PKG_MD5SUM:=c389d3ffba0e69a179de2ec650f1fdcc

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(FPIC)

define Package/howl/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Zeroconf networking implementation
  URL:=http://www.porchdogsoft.com/products/howl/
endef

define Package/libhowl
  $(call Package/howl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE+= (library)
endef

define Package/libhowl/description
	Howl is a cross-platform implementation of Zeroconf networking.	
	This package contains the Howl shared libraries, used by other programs.
endef

define Package/howl-autoipd
  $(call Package/howl/Default)
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libhowl +libpthread
  TITLE:=An IPv4 Link-Local address self-assigner daemon
endef

define Package/howl-autoipd/description
	Howl is a cross-platform implementation of Zeroconf networking.	
	This package contains an IPv4 Link-Local address self-assigner daemon.
endef

define Package/howl-mdnsresponder
  $(call Package/howl/Default)
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libhowl +libpthread
  TITLE:=An mDNS (Multicast DNS) service responder daemon
endef

define Package/howl-mdnsresponder/description
	Howl is a cross-platform implementation of Zeroconf networking.	
	This package contains an mDNS (Multicast DNS) service responder daemon.
endef

define Package/howl-mdnsresponder/conffiles
/etc/mDNSResponder.conf
endef

define Package/howl-nifd
  $(call Package/howl/Default)
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libhowl +libpthread
  TITLE:=A network interface monitor daemon
endef

define Package/howl-nifd/description
	This package contains a network interface monitor daemon.
endef

define Package/howl-utils
  $(call Package/howl/Default)
  SUBMENU:=IP Addresses and Names
  DEPENDS:=+libhowl +libpthread
  TITLE+= (utilities)
endef

define Package/howl-utils/description
	This package contains mDNS (Multicast DNS) client utilities.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
	);
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/howl $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{howl,mDNSResponder}.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/howl.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-I/usr/include,-I$$$${includedir},g' $(1)/usr/lib/pkgconfig/howl.pc
	$(SED) 's,-L/usr/lib/howl,-L$$$${libdir}/howl,g' $(1)/usr/lib/pkgconfig/howl.pc
endef

define Package/libhowl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{howl,mDNSResponder}.so.* $(1)/usr/lib/
endef

define Package/howl-autoipd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/autoipd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/autoipd.init $(1)/etc/init.d/autoipd
endef

define Package/howl-mdnsresponder/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mDNSResponder $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/mDNSResponder.conf $(1)/etc/mDNSResponder.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/mDNSResponder.init $(1)/etc/init.d/mDNSResponder
endef

define Package/howl-nifd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/nifd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/nifd.init $(1)/etc/init.d/nifd
endef

define Package/howl-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mDNS{Browse,Publish,Query,Resolve} $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libhowl))
$(eval $(call BuildPackage,howl-autoipd))
$(eval $(call BuildPackage,howl-mdnsresponder))
$(eval $(call BuildPackage,howl-nifd))
$(eval $(call BuildPackage,howl-utils))
