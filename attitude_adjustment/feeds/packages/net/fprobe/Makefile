#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fprobe
PKG_VERSION:=1.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/fprobe
PKG_MD5SUM:=65850d0470078269b33eee58cba77ac2

include $(INCLUDE_DIR)/package.mk

define Package/fprobe
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libpcap +libpthread
  TITLE:=NetFlow probe (libpcap-based)
  URL:=http://fprobe.sourceforge.net/
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--with-piddir=/var \
		--with-pcap="$(STAGING_DIR)/usr/include" \
		--with-libpcap="$(STAGING_DIR)/usr/lib" \
		--with-membulk=index8 \
		--with-hash=xor8 \
	)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/fprobe/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/fprobe $(1)/usr/bin/
endef

$(eval $(call BuildPackage,fprobe))
