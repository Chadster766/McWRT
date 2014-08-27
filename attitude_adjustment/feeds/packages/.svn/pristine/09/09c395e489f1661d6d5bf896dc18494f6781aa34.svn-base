#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wiviz
PKG_VERSION:=1.0
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/wiviz
  SECTION:=net
  CATEGORY:=Network
  SUMENU:=wireless
  DEPENDS:=+libpcap
  TITLE:=Wireless Network Visualization
  URL:=http://devices.natetrue.com/wiviz/
endef
	
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

EXTRA_CFLAGS += $(TARGET_CPPFLAGS)
EXTRA_LDFLAGS += $(TARGET_LDFLAGS)

define Build/Compile
	$(call Build/Compile/Default, \
		CCOPTS="$(TARGET_CFLAGS)" \
		INCLUDE="$(EXTRA_CFLAGS)" \
		LDFLAGS="$(EXTRA_LDFLAGS)" \
	)
endef

define Package/wiviz/install	
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/sbin/
	$(CP) ./files/* $(1)
	find $(1) -name CVS | xargs rm -rf
	find $(1) -name .svn | xargs rm -rf
endef

$(eval $(call BuildPackage,wiviz))
