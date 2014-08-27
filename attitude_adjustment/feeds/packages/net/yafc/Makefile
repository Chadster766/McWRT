#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yafc
PKG_VERSION:=1.1.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=832d074183a36ee15b47553ed5962fce

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

include $(INCLUDE_DIR)/package.mk

define Package/yafc
  SUBMENU:=File Transfer
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses +libreadline
  TITLE:=Yafc is yet another ftp client.
  URL:=http://yafc.sourceforge.net/
endef

# uses GNU configure
CONFIGURE_ARGS += \
	--with-socks=no \
	--with-socks5=no \
	--with-krb4=no \
	--with-krb5=no \
	--with-gssapi=no

CONFIGURE_VARS += \
	bash_cv_func_sigsetjmp=missing

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/yafc/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/yafc $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,yafc))
