include $(TOPDIR)/rules.mk

PKG_NAME:=libcroco
PKG_VERSION:=0.6.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNOME/$(PKG_NAME)/0.6/
PKG_MD5SUM:=1429c597aa4b75fc610ab3a542c99209
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

EXTRA_LDFLAGS+=-Wl,-rpath-link=$(STAGING_DIR)/usr/lib \
               -L$(ICONV_PREFIX) -L$(INTL_PREFIX)

define Package/libcroco
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Croco css2 parsing libraray.
  DEPENDS:=+glib2 +libxml2
  URL:=http://www.freespiders.org/projects/libcroco/
endef

define Package/libcroco/description
Libcroco is a standalone css2 parsing and manipulation library.
The parser provides a low level event driven SAC like api
and a css object model like api.
Libcroco provides a CSS2 selection engine and an experimental
xml/css rendering engine.

This library is being written to bring the css support
to the mlview xml editor project but it can be used
for other applications as well.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR)/ install DESTDIR=$(PKG_INSTALL_DIR)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/* $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/host/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/croco-0.6-config $(1)/host/bin/
endef

define Package/libcroco/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*{a,so*} $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libcroco))


