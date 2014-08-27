#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=cmdftp
PKG_VERSION:=0.9.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://download.savannah.nongnu.org/releases/$(PKG_NAME)
PKG_MD5SUM:=9bd0741bbb1f218be95c089dc06b2777

include $(INCLUDE_DIR)/package.mk

define Package/cmdftp
  SUBMENU:=File Transfer
  SECTION:=net
  CATEGORY:=Network
  TITLE:=cmdftp is a tiny command line FTP client for Unix systems
  URL:=http://www.nongnu.org/cmdftp/
endef

define Package/cmdftp/description
	Features include passive mode for all data transfers, shell
	like transparent syntax for local and remote modes, multiple
	and recursive file transfers using wildcards, recursive copy and
	move commands, remote and local text file viewing and editing,
	network errors detection and resuming of currently executing
	command, partial download resuming (if server accepts REST
	command), tab completion for both local and remote names,
	autologin using classic ~/.netrc approach, large file support.

	cmdftp is aimed at being small and simple.
endef

define Package/cmdftp/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
endef

$(eval $(call BuildPackage,cmdftp))
