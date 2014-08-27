#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=lksctp-tools
PKG_VERSION:=1.0.11
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/lksctp/
PKG_MD5SUM:=e9cf6c57402c9d4f1173a9529466e16d

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

#####
## lksctp library: provides sctp_* functions in user space
#####

define Package/sctp
  SECTION:=libs
  CATEGORY:=Network
  TITLE:=SCTP user-land library
  URL:=http://lksctp.sourceforge.net
  DEPENDS:=+kmod-sctp
endef

define Package/sctp/description
 SCTP user-land library for sctp_* functions (when socket API is too limited).
endef

TARGET_LDFLAGS := -L$(STAGING_DIR)/usr/lib $(TARGET_LDFLAGS)

define Package/sctp/install
	# Install the library in the router
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libsctp.so* \
		$(1)/usr/lib/
	# And also in staging for dependent packages
	$(INSTALL_DIR) $(STAGING_DIR)/usr/include/netinet
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/include/netinet/* \
		$(STAGING_DIR)/usr/include/netinet/
	$(INSTALL_DIR) $(STAGING_DIR)/usr/lib
	$(CP) \
		$(PKG_INSTALL_DIR)/usr/lib/libsctp.so* \
		$(STAGING_DIR)/usr/lib/

endef

$(eval $(call BuildPackage,sctp))

