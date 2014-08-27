#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=diffutils
PKG_VERSION:=3.0
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/diffutils
PKG_MD5SUM:=684aaba1baab743a2a90e52162ff07da

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/diffutils
  SECTION:=devel
  CATEGORY:=Development
  DEPENDS:=+USE_EGLIBC:librt
  TITLE:=diffutils
  URL:=http://www.gnu.org/software/diffutils/
endef

define Package/diffutils/description
  The Diffutils package contains programs that show the differences between
  files or directories.
endef

CONFIGURE_VARS += \
	ac_cv_func_mempcpy=n \

define Package/diffutils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/{sdiff,diff3,diff,cmp} $(1)/usr/bin/
endef

define Package/diffutils/preinst
#!/bin/sh
for x in sdiff diff3 diff cmp; do
  [ -L "$${IPKG_INSTROOT}/usr/bin/$$x" ] && rm -f "$${IPKG_INSTROOT}/usr/bin/$$x"
done
exit 0
endef

define Package/diffutils/postrm
#!/bin/sh
for x in sdiff diff3 diff cmp; do
  /bin/busybox $$x -h 2>&1 | grep -q BusyBox && ln -sf ../../bin/busybox /usr/bin/$$x
done
exit 0
endef

$(eval $(call BuildPackage,diffutils))
