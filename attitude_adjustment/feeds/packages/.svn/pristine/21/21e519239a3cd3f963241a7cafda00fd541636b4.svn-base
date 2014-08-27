#
# Copyright (C) 2009-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bash-completion
PKG_VERSION:=20080705
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://ftp.de.debian.org/debian/pool/main/b/bash-completion/
PKG_MD5SUM:=8bc36281bb0567bc67a9083c4c5595a9

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/bash-completion
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=bash completion scripts
  DEPENDS:=+libncurses +bash
  URL:=http://www.caliban.org/bash/
  PKGARCH:=all
endef

define Package/bash-completion/description
  bash completion extends bashs standard completion behavior
  to achieve complex command lines with just a few keystrokes.
endef


define Build/Configure
endef


define Build/Compile
endef

define Package/bash-completion/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bash_completion $(1)/etc/
endef

define Package/bash-completion/conffiles
/etc/bash_completion
endef

$(eval $(call BuildPackage,bash-completion))
