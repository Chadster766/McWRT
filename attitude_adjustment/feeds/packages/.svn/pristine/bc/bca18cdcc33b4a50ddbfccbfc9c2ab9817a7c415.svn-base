#
# This is free software, licensed under the GNU General Public License v2.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=4th
PKG_VERSION:=3.61.5
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-nano.tar.gz
PKG_SOURCE_URL:=http://4th.googlecode.com/files/
PKG_MD5SUM:=2d6ddad0e595a7549a425ce33cebd830

include $(INCLUDE_DIR)/package.mk

define Package/4th
  MAINTAINER:="Xiangfu Liu" <xiangfu@sharism.cc>
  TITLE:=4th
  SECTION:=lang
  CATEGORY:=Languages
  URL:=http://thebeez.home.xs4all.nl/4tH/
endef

define Package/4th/description
  4th forth compiler
endef

MAKE_VARS += \
	LIBRARIES="${PKG_INSTALL_DIR}/usr/lib" \
	BINARIES="${PKG_INSTALL_DIR}/usr/bin"
MAKE_FLAGS += \
	-C $(PKG_BUILD_DIR)/sources -f Makefile.NAN

define Package/4th/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sources/4th $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sources/4tsh $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sources/pp4th $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/sources/lib4th.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,4th))
