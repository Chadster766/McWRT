# 
# Copyright (C) 2009 David Cooper <dave@kupesoft.com>
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sslsniff
PKG_VERSION:=0.5
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.thoughtcrime.org/software/sslsniff
PKG_MD5SUM:=24653e2c4f1ca94328520667eb756369

include $(INCLUDE_DIR)/package.mk

define Package/sslsniff
  SECTION:=net
  CATEGORY:=Network
  TITLE:=SSL man-in-the-middle attack
  URL:=http://www.thoughtcrime.org/software/sslsniff/index.html
  DEPENDS:=+libopenssl
endef

define Package/sslsniff/description
 sslsniff is designed to MITM all SSL connections on a LAN,
 and dynamically generates certs for the domains that are
 being accessed on the fly. The new certificates are constructed
 in a certificate chain that is signed by any certificate that
 you provide.
endef

# Take this opportunity to remove the pre-packaged x86 binary
define Build/Prepare
$(call Build/Prepare/Default)
	$(RM) $(PKG_BUILD_DIR)/sslsniff
endef

MAKE_FLAGS += CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS)"

define Package/sslsniff/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sslsniff $(1)/usr/bin/
endef

$(eval $(call BuildPackage,sslsniff))

