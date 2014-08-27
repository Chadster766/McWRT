#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=msr-tools
PKG_VERSION:=1.1.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.kernel.org/pub/linux/utils/cpu/msr-tools
PKG_MD5SUM:=2452d2233556aa0fb8b53208361a2c94

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -fomit-frame-pointer -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64

define Package/msr-tools
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=
  TITLE:=x86 MSR utilities.
  URL:=http://www.kernel.org/pub/linux/utils/cpu/msr-tools
  SUBMENU:=CPU tools
endef

define Package/msr-tools/description
	Manipulate the MSR (machine state registers) on i386 and x86_64
	processors.
endef

define Build/Configure
endef

define Build/Compile
	$(INSTALL_DIR) "$(PKG_INSTALL_DIR)/usr/sbin"
	$(MAKE) -C $(PKG_BUILD_DIR) all \
		$(TARGET_CONFIGURE_OPTS) \
		sbindir="$(PKG_INSTALL_DIR)/usr/sbin" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="" \
		install
endef

define Package/msr-tools/install	
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/* $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,msr-tools))
