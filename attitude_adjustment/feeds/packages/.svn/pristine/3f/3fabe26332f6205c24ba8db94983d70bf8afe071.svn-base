#
# Copyright (C) 2010-2011 OpenWrt.org
# Copyright (C) 2010 Gianluigi Tiesi <sherpya@netfarm.it>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=axel
PKG_VERSION:=2.4
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://alioth.debian.org/frs/download.php/3015
PKG_MD5SUM:=a2a762fce0c96781965c8f9786a3d09d

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/axel
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=File Transfer
  TITLE:=Axel Download Accelerator
  DEPENDS:=+libpthread
  URL:=http://axel.alioth.debian.org/
  MAINTAINER:=Gianluigi Tiesi <sherpya@netfarm.it>
endef

define Package/axel/description
  Axel tries to accelerate HTTP/FTP downloading process by using multiple connections for one file.
  It can use multiple mirrors for a download. Axel has no dependencies and is lightweight,
  so it might be useful as a wget clone on byte-critical systems.
endef

# notes:
#  - I'm using := and not += because it is not a standard configure script
#  - I ask not to strip, because it should be handled by the toolchain
CONFIGURE_ARGS := \
	--prefix=/usr \
	--etcdir=/etc \
	--debug=0 \
	--i18n=0 \
	--strip=0

define Package/axel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/axel $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/axelrc.example $(1)/etc/axelrc
	echo "alternate_output = 1" >> $(1)/etc/axelrc
endef

define Package/axel/conffiles
/etc/axelrc
endef

$(eval $(call BuildPackage,axel))
