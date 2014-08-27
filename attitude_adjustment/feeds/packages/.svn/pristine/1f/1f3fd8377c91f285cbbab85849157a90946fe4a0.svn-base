#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bitchx
PKG_VERSION:=1.1
PKG_RELEASE:=1

PKG_SOURCE:=ircii-pana-$(PKG_VERSION)-final.tar.gz
PKG_SOURCE_URL:=http://bitchx.org/files/source/
PKG_MD5SUM:=611d2dda222f00c10140236f4c331572

PKG_BUILD_DIR:=$(BUILD_DIR)/BitchX

PKG_BUILD_DEPENDS:=libncurses

include $(INCLUDE_DIR)/package.mk

define Package/bitchx
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses
  TITLE:=popular IRC client
  URL:=http://www.bitchx.org
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	mkdir -p $(PKG_INSTALL_DIR)/tmp
	mkdir -p $(PKG_INSTALL_DIR)/etc/init.d
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx
	mkdir -p $(PKG_INSTALL_DIR)/usr/share/man/man1
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/translation
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/plugins
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/bx/help
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/source/BitchX $(PKG_INSTALL_DIR)/usr/bin/BitchX-1.1-final
	rm -f $(PKG_INSTALL_DIR)/usr/bin/BitchX
	ln -s /usr/bin/BitchX-1.1-final $(PKG_INSTALL_DIR)/usr/bin/BitchX 
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/BitchX.help $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.help
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/BitchX.ircnames $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.ircnames
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/BitchX.quit $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.quit
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/BitchX.reasons $(PKG_INSTALL_DIR)/usr/lib/bx/BitchX.kick
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/source/wserv $(PKG_INSTALL_DIR)/usr/lib/bx/wserv
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/source/scr-bx $(PKG_INSTALL_DIR)/usr/bin/scr-bx
	$(CP) $(PKG_BUILD_DIR)/script $(PKG_INSTALL_DIR)/usr/lib/bx/
	$(CP) $(PKG_BUILD_DIR)/translation $(PKG_INSTALL_DIR)/usr/lib/bx/
	bzip2 $(PKG_INSTALL_DIR)/usr/lib/bx/translation/*
	$(CP) ./files/bitchxrc $(PKG_INSTALL_DIR)/tmp/.bitchxrc
	$(CP) ./files/bitchxrc.init $(PKG_INSTALL_DIR)/etc/init.d/bitchxrc
endef

define Package/bitchx/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/lib $(1)/etc/init.d $(1)/tmp
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/etc/init.d/bitchxrc $(1)/etc/init.d/
	$(CP) $(PKG_INSTALL_DIR)/tmp/.bitchxrc $(1)/tmp
endef

$(eval $(call BuildPackage,bitchx))
