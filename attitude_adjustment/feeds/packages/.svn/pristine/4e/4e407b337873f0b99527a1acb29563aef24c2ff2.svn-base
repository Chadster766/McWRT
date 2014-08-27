#
# Copyright (C) 2009 Freifunk Leipzig
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ices
PKG_VERSION:=2.0.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.xiph.org/releases/ices/
PKG_MD5SUM:=0d95ac34d59fed337028d5e7771076d8
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/ices
  SECTION:=sound
  CATEGORY:=Sound
  TITLE:=ices client for Icecast media streaming servers
  URL:=http://xiph.org/
  DEPENDS:=+libshout +libxml2 +zlib +libogg +libvorbis +alsa-lib
endef

define Package/ices/description
  ices is a command line source client for Icecast media streaming servers.
  It began as the successor of the old "shout" utility, and has since gained a
  lot of useful features.
endef

define Package/ices/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/bin/
endef

CONFIGURE_ARGS+= \
    --with-ogg="$(STAGING_DIR)/usr/include" \
    --with-vorbis="$(STAGING_DIR)/usr/include" \

define Build/Configure
    $(call Build/Configure/Default, \
      LDFLAGS="$(TARGET_LDFLAGS) -lz -lm" \
    )
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  $(TARGET_CONFIGURE_OPTS) \
	  CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
	  LDFLAGS="$(TARGET_LDFLAGS) -lz -lm"
endef

$(eval $(call BuildPackage,ices)) 
