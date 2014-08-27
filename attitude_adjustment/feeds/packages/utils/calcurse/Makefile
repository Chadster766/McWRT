include $(TOPDIR)/rules.mk

PKG_NAME:=calcurse
PKG_VERSION:=2.8
PKG_RELEASE:=1

PKG_SOURCE:=get.cgi?calcurse-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://culot.org/cgi-bin
PKG_MD5SUM:=124d40c0912b12021996220f02b759cc
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/calcurse-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/calcurse
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Calcurse
  DEPENDS:=+libncursesw +libreadline +libpthread
  URL:=http://culot.org/calcurse
endef

define Package/calcurse/description
	Fullscreen curses based calendar.
endef

define Build/Configure
	$(call Build/Configure/Default, \
		--prefix=/usr \
	)
endef

define Package/calcurse/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/calcurse $(1)/usr/bin/
endef

$(eval $(call BuildPackage,calcurse))
