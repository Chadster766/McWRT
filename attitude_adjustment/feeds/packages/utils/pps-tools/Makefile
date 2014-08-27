#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=pps-tools
PKG_VERSION:=2.6.23
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/pps-tools
  SUBMENU:=Time Synchronization
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@LINUX_2_6_23
  TITLE:=PPS (Pulse Per Second) Userland tools
  URL:=http://wiki.enneenne.com/index.php/LinuxPPS_support
endef


define Package/pps-tools/description
  Userland tools for GPS and DCF-77 Clock syncronization. PPS support in Kernel must be enabled.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/timepps.h $(1)/usr/include/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DHAVE_ISC_READER=1 -I$(LINUX_DIR)/include -I ." \
		BINDIR="/usr/sbin" MANDIR="/usr/man"
endef

define Package/pps-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ppstest $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ppsctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,pps-tools))
