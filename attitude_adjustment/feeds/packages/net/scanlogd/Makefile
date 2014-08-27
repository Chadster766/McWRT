#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=scanlogd
PKG_VERSION:=2.2.6
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openwall.com/scanlogd/ \
	ftp://ftp.wiretapped.net/pub/openwall/ \
	http://distro.ibiblio.org/pub/linux/distributions/openwall/
PKG_MD5SUM:=7b8187ea718ebe47f22805b921b909ab

PKG_INIT_PRIO:=60

include $(INCLUDE_DIR)/package.mk

define Package/scanlogd
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  DEPENDS:=+libpcap +libnet0 +libnids
  TITLE:=Port scan detection tool
  URL:=http://www.openwall.com/scanlogd
endef

define Package/scanlogd/description
	Scanlogd is a TCP port scan detection tool, originally designed 
	to illustrate various attacks an IDS developer has to deal with.
	Thus, unlike some of the other port scan detection tools out there, 
	scanlogd is designed to be totally safe to use.
endef

define Build/Configure	
	$(SED) 's,^#define \(SCANLOGD_CHROOT\).*,#define \1 "/var/run/scanlogd",' \
	    -e 's,^#undef \(SCANLOGD_DEVICE\),#define \1 "all",' \
	    $(PKG_BUILD_DIR)/params.h
endef

define Build/Compile	
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC="$(TARGET_CC)" LD="$(TARGET_CC)" CFLAGS="-c $(TARGET_CFLAGS)" \
		LDFLAGS= \
		PCAP_H="$(TARGET_CPPFLAGS)" \
		NIDS_H="$(TARGET_CPPFLAGS) -I$(STAGING_DIR)/usr/lib/libnet-1.0.x/include" \
		NIDS_L="$(TARGET_LDFLAGS) -L$(STAGING_DIR)/usr/lib/libnet-1.0.x/lib -lnids -lnet -lpcap" \
		libnids
endef

define Package/scanlogd/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/scanlogd.init $(1)/etc/init.d/scanlogd
endef

$(eval $(call BuildPackage,scanlogd))
