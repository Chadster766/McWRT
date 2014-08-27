# 
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gperf
PKG_VERSION:=3.0.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/gperf
PKG_MD5SUM:=cc20e58975a38075440423c8fb85fd00
PKG_HOST_ONLY=1

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/package.mk

define Package/gperf
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Perfect hash function generator
  BUILDONLY:=1
  URL:=http://www.gnu.org/software/gperf
endef

define Package/lua/Default/description
 gperf is a perfect hash function generator
endef

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) install
endef


$(eval $(call HostBuild))
$(eval $(call BuildPackage,gperf))
