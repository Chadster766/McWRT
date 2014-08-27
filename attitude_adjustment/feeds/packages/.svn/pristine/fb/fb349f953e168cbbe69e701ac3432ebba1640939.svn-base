#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=maemo-kexec
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=maemo-rx44-2008.43-7.kernel
PKG_SOURCE_URL:=http://bu3sch.de/misc/
PKG_MD5SUM:=3cbf571c0618bfe20a69babab6b33bea
UNPACK_CMD=$(CP) $(DL_DIR)/$(PKG_SOURCE) $(PKG_BUILD_DIR)/

PKG_CONFIG_DEPENDS:= \
	CONFIG_MAEMO_KEXEC_DOWNLOAD_KERNEL \
	CONFIG_MAEMO_KEXEC_KERNEL_PATH

include $(INCLUDE_DIR)/package.mk

define Package/maemo-kexec
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_omap24xx +kexec-tools
  TITLE:=Kexec Maemo from OpenWRT
endef

define Package/maemo-kexec/description
 Kexec Maemo from OpenWRT.
 The Maemo rootfs must be in the rootfs MTD partition.
endef

define Package/maemo-kexec/config
	source "$(SOURCE)/Config.in"
endef

define Build/Configure
endef

define Build/Compile
endef

MAEMO_KRNL_SRC:=$(if $(CONFIG_MAEMO_KEXEC_DOWNLOAD_KERNEL),\
		     $(PKG_BUILD_DIR)/$(PKG_SOURCE),\
		     $(CONFIG_MAEMO_KEXEC_KERNEL_PATH))

define Package/maemo-kexec/install
	$(INSTALL_DIR) $(1)/boot $(1)/sbin
	$(INSTALL_BIN) ./files/sbin/bootmaemo $(1)/sbin/
	$(INSTALL_DATA) $(MAEMO_KRNL_SRC) $(1)/boot/maemo.zImage
endef

$(eval $(call BuildPackage,maemo-kexec))
