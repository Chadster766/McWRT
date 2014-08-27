#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tig
PKG_VERSION:=0.18
PKG_RELEASE:=1

PKG_SOURCE:=tig-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://jonas.nitro.dk/tig/releases/
PKG_MD5SUM:=4fa9e33c5daa76b6fed11e068405356f
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/tig-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=libncurses libiconv

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/tig
	SECTION:=devel
	CATEGORY:=Development
	TITLE:=Console Git repository viewer
	DEPENDS:=+git +libncursesw
	URL:=http://jonas.nitro.dk/tig/releases/
endef

define Package/tig/description
	Command line launcher for search engines and other Internet services.
endef

CONFIGURE_ARGS+= \
	--with-libiconv="$(ICONV_PREFIX)"

define Package/tig/install
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tig $(1)/usr/bin/tig
endef

$(eval $(call BuildPackage,tig))
