# 
# Copyright (C) 2008-2011 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gw6c
PKG_VERSION:=5.1
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://openwrt.linux-appliance.net/sources
PKG_MD5SUM:=eeac7292a622681651ec3bd9b2e5b061
PKG_BUILD_DIR:=$(BUILD_DIR)/gw6-$(PKG_VERSION)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/gw6c
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+libpthread +ip +kmod-tun +kmod-ipv6 $(CXX_DEPENDS) +radvd
  TITLE:=IPv6 tunnel client software
  URL:=http://www.go6.net
endef

define Package/gw6c/description
  Client to configure an IPv6 tunnel to
  Hexago/freenet6/go6.net's migration broker.
endef

GW6C_OPTS:= \
	C_COMPILER="$(TARGET_CC) -c -DNO_STDLIBCXX" \
	COMPILER="$(TARGET_CXX) -c -DNO_STDLIBCXX" \
	CPP_FLAGS="$(TARGET_CFLAGS) -I. -Wall -DNDEBUG  $(TARGET_CPPFLAGS)" \
	RANLIB="$(TARGET_CROSS)ranlib" \
	ARCHIVER="$(TARGET_CROSS)ar" \
	C_LINKER="$(TARGET_CC)" \
	LINKER="$(TARGET_CXX)" \
	exportdir=$(PKG_BUILD_DIR)/gw6c \
	export

define Build/Configure
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/gw6c/include
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/gw6c-config/gw6cconfig/gw6c_c_wrapper.h \
		$(PKG_BUILD_DIR)/gw6c/include
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/gw6c-config \
		$(GW6C_OPTS)
	$(MAKE) -C $(PKG_BUILD_DIR)/gw6c-messaging \
		$(GW6C_OPTS)
	$(MAKE) -C $(PKG_BUILD_DIR)/tspc-advanced \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -fno-builtin -Wall -I$(PKG_BUILD_DIR)/tspc-advanced/include \
			-I$(PKG_BUILD_DIR)/tspc-advanced/platform/openwrt -I$(PKG_BUILD_DIR)/tspc-advanced" \
		LDFLAGS="-L$(PKG_BUILD_DIR)/tspc-advanced/gw6cconfig \
			-L$(PKG_BUILD_DIR)/tspc-advanced/gw6cmessaging \
			$(TARGET_LDFLAGS) $(LIBGCC_S)" \
		target="openwrt" \
		installdir="$(PKG_INSTALL_DIR)"
endef

define Package/gw6c/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tspc-advanced/bin/$(PKG_NAME) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/share/gw6c/template
	$(CP) ./files/openwrt.sh $(1)/usr/share/gw6c/template
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/gw6c.init $(1)/etc/init.d/gw6c
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/gw6c.config $(1)/etc/config/gw6c
endef

define Package/gw6c/conffiles
/etc/config/gw6c
endef

$(eval $(call BuildPackage,gw6c))
