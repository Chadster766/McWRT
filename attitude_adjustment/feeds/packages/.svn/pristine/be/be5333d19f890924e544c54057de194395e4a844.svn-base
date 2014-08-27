#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=strace

PKG_VERSION:=4.5.20
PKG_RELEASE:=1
PKG_MD5SUM:=64dfe10d9db0c1e34030891695ffca4b

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/$(PKG_NAME)

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/strace
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=System call tracer
  URL:=http://strace.sourceforge.net/
endef

define Package/strace/description
A useful diagnostic, instructional, and debugging tool. Allows you to track what
system calls a program makes while it is running.
endef

# needed when compiling against Linux 2.4
CONFIGURE_VARS += \
	ac_cv_header_linux_netlink_h=yes

ifneq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
  TARGET_CPPFLAGS += -I$(LINUX_DIR)/include
endif

MAKE_FLAGS := \
	CCOPT="$(TARGET_CFLAGS)"

define Package/strace/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/strace $(1)/usr/bin/
endef

$(eval $(call BuildPackage,strace))
