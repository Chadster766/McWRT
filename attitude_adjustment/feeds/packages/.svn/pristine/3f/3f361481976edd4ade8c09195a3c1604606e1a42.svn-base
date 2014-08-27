#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=send
PKG_VERSION:=0.2-5.4
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)d_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mobisend.org/debian-mobisend/pool/main/s/sendd/
PKG_MD5SUM:=99c18e5d965e57d580f146d00145251a
PKG_BUILD_DIR:=$(BUILD_DIR)/sendd-0.2

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/send
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=Secure Neighbor Discovery implementation
  URL:=http://mobisend.org/debian-mobisend.html
  DEPENDS:=+kmod-ipv6 +ip6tables +libnetfilter-queue +libopenssl +libreadline +libncurses
endef

define Package/send/description
	DoCoMo's Open Source SEND project provides an implementation of RFC 3971
	Secure Neighbor Discovery (SEND). SEND cryptographically secures the
	IPv6 neighbor discovery protocol, countering the threats discussed in
	RFC 3756 (IPv6 Neighbor Discovery (ND) Trust Models and Threats).
endef

define Package/send/conffiles
/etc/sendd/sendd.conf
/etc/sendd/params.conf
endef

TARGET_CPPFLAGS+=-D_GNU_SOURCE

MAKE_FLAGS += \
	CC="$(TARGET_CC)" \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnetfilter_queue $(TARGET_CPPFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS) -lnfnetlink" \

define Package/send/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sendd/sendd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/sendd/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/examples/sendd.conf $(1)/etc/sendd/
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/examples/params.conf $(1)/etc/sendd/
endef

$(eval $(call BuildPackage,send))
