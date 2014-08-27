#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rsyncrypto
PKG_VERSION:=1.12
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rsyncrypto
PKG_MD5SUM:=62367d5458ceaab58bbe73030880f538

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rsyncrypto
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Rsync friendly encryption
  URL:=http://rsyncrypto.lingnu.com/
  DEPENDS:=+argtable +gzip +libopenssl +libstdcpp
endef

define Package/rsyncrypto/description
  rsyncrypto is a utility that encrypts a file (or a directory structure) in a
  way that ensures that local changes to the plain text file will result in
  local changes to the cipher text file. This, in turn, ensures that doing rsync
  to synchronize the encrypted files to another machine will have only a small
  impact on rsync's efficiency.
endef

CONFIGURE_ARGS += \
	--with-openssl="$(STAGING_DIR)/usr"

define Package/rsyncrypto/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/rsyncrypto $(1)/usr/bin/
endef

$(eval $(call BuildPackage,rsyncrypto))
