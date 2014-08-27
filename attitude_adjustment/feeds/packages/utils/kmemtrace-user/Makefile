#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=kmemtrace-user
PKG_VERSION:=20100102
PKG_RELEASE:=1
PKG_REV:=92878e602489d6a6ffd6e50b94f425c3149c86bd

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://repo.or.cz/kmemtrace-user.git
PKG_SOURCE_SUBDIR:=kmemtrace-user-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/kmemtrace-user
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=trace slab allocator functions
  URL:=http://repo.or.cz/w/kmemtrace-user.git
  DEPENDS:=+libpthread
  KCONFIG:= \
	CONFIG_KMEMTRACE=y
endef

define Package/kmemtrace-user/description
 kmemtrace provides tracing for slab allocator functions, such as
 kmalloc, kfree, kmem_cache_alloc, kmem_cache_free etc.. Collected
 data is then fed to the userspace application in order to analyse
 allocation hotspots, internal fragmentation and so on, making it
 possible to see how well an allocator performs, as well as debug
 and profile kernel code.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); ./autogen.sh; ./configure );
	$(call Build/Configure/Default)
endef


define Package/kmemtrace-user/install    
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kmemtrace-check $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kmemtrace-report $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kmemtrace-show $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/kmemtraced $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,kmemtrace-user))
