#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mutella
PKG_VERSION:=0.4.5
PKG_RELEASE:=4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=1a676eacf562e3b8de90493f99fe059c

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/mutella
  SUBMENU:=P2P
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Gnutella client with command line and http interface
  URL:=http://mutella.sourceforge.net/
  DEPENDS:=+libncurses +libreadline +libpthread +libstdcpp +zlib
endef

define Package/mutella/description
 Mutella supports all the functionality required to participate as
 a full-featured node in the Gnutella network. "Full-featured" implies
 support for file search, download and sharing. It is optimized for a
 high-bandwidth connection, where it sets standards for server
 performance and stability, but can also run on a modest-speed line.
 .
 Mutella can either be controlled via a colourful text mode interface
 or via a web browser using a built in http interface.
endef

CONFIGURE_VARS += \
	CXXFLAGS="$$$$CXXFLAGS -fno-rtti"  \

define Package/mutella/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mutella{,_sio} $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) $(PKG_INSTALL_DIR)/usr/share/mutella $(1)/usr/share/
endef

$(eval $(call BuildPackage,mutella))

