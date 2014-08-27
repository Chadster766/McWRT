#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=time
PKG_VERSION:=1.7
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/time
PKG_MD5SUM:=e38d2b8b34b1ca259cf7b053caac32b3

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/time
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=The GNU time utility
  URL:=http://www.gnu.org/software/time/
endef

define Package/time/description
  `time' is a program that measures many of the CPU resources, such as time and
  memory, that other programs use.  The GNU version can format the output in
  arbitrary ways by using a printf-style format string to include various
  resource measurements.  Some systems do not provide much information about
  program resource use; `time' reports unavailable information as zero values.
endef

define Package/time/preinst
#!/bin/sh
if [ -e $${IPKG_INSTROOT}/usr/bin/time ]; then
  rm $${IPKG_INSTROOT}/usr/bin/time;
fi
endef

define Package/time/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/time $(1)/usr/bin/
endef

define Package/time/postrm
#!/bin/sh
ln -sf ../../bin/busybox $${IPKG_INSTROOT}/usr/bin/time
$${IPKG_INSTROOT}/usr/bin/time 2>&1 | grep 'applet not found' > /dev/null 2>&1 && rm $${IPKG_INSTROOT}/usr/bin/time
exit 0
endef

$(eval $(call BuildPackage,time))
