#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=shadow
PKG_VERSION:=4.1.5.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://pkg-shadow.alioth.debian.org/releases
PKG_MD5SUM:=a00449aa439c69287b6d472191dc2247

PKG_FIXUP:=autoreconf

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

UTILS := \
	groupadd groupdel groupmod useradd userdel usermod \
	su passwd groups

CONFIGURE_ARGS += \
	--without-audit \
	--without-libpam \
	--without-selinux \
	--without-acl \
	--without-attr \
	--without-tcb \
	--without-nscd

define Package/shadow
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=The PLD Linux shadow utilities
  URL:=http://pkg-shadow.alioth.debian.org/
  DEPENDS:=$(foreach u,$(UTILS),+shadow-$(u))
endef

define Package/shadow/description
  Full versions of standard shadow utilities.  Normally, you would not
  use this package, since the functionality in BusyBox is more than
  sufficient and much smaller.
endef

define Package/shadow/install
	true
endef


define Package/shadow-common
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Shared definitions for the shadow utilities
  URL:=http://pkg-shadow.alioth.debian.org/
endef

define Package/shadow-common/conffiles
/etc/login.defs
endef

define Package/shadow-common/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/etc/login.defs $(1)/etc/
endef


define program
  define Package/shadow-$(1)
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=Utility $(1) from the PLD Linux shadow utilities
    URL:=http://pkg-shadow.alioth.debian.org/
    DEPENDS:=+shadow-common
  endef

  define Package/shadow-$(1)/description
    Full version of standard $(1) utility. Normally, you would not use this
    package, since the functionality in BusyBox is more than sufficient.
  endef

  define Package/shadow-$(1)/install
	[ -x "$(PKG_INSTALL_DIR)/usr/sbin/$(1)" ] && { \
		$(INSTALL_DIR) $$(1)/usr/sbin; \
		$(CP) $(PKG_INSTALL_DIR)/usr/sbin/$(1) $$(1)/usr/sbin/; \
	} || { \
		$(INSTALL_DIR) $$(1)/usr/bin; \
		$(CP) $(PKG_INSTALL_DIR)/usr/bin/$(1) $$(1)/usr/bin/; \
	}
  endef

  $$(eval $$(call BuildPackage,shadow-$(1)))
endef

$(eval $(call BuildPackage,shadow))
$(eval $(call BuildPackage,shadow-common))
$(foreach u,$(UTILS),$(eval $(call program,$(u))))
