#
# Copyright (C) 2007-2011 OpenWrt.org
# Copyright (c) 2009 Thorsten Glaser <tg@mirbsd.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mksh
PKG_VERSION:=40d
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-R$(PKG_VERSION).cpio.gz
PKG_SOURCE_URL:=http://www.mirbsd.org/MirOS/dist/mir/mksh
PKG_MD5SUM:=c6428401103367730a95b99284bf47dc

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/mksh
  SECTION:=shells
  CATEGORY:=Base system
  TITLE:=MirBSD Korn Shell
  DEPENDS:=$(DEP)
  URL:=http://mirbsd.de/mksh
endef

define Package/mksh/description
mksh is the MirBSD enhanced version of the Public Domain Korn
shell (pdksh), a Bourne-compatible shell which is largely si-
milar to the original AT&T Korn shell; mksh is the only pdksh
derivate currently being actively developed.  It includes bug
fixes and feature improvements, in order to produce a modern,
robust shell good for interactive and especially script use.
mksh has UTF-8 support (in substring operations and the Emacs
editing mode) and, while R40d corresponds to OpenBSD 5.0-cur-
rent ksh (without GNU bash-like PS1 and fancy character clas-
ses), adheres to SUSv4 and is much more robust.  The code has
been cleaned up and simplified, bugs fixed, standards compli-
ance added, and several enhancements (for extended compatibi-
lity to other modern shells - as well as a couple of its own)
are available. It has sensible defaults as usual with BSD.
endef

define Build/Compile
	# -DMKSH_SMALL=1 ⇒ reduce functionality quite a lot
	# -DMKSH_ASSUME_UTF8=0 ⇒ never automatically enable
	#	UTF-8 mode, neither use setlocale/nl_langinfo
	#	nor look at $LC_* and $LANG (not recommended)
	# HAVE_CAN_FSTACKPROTECTORALL=0 ⇒ nuke libssp dependency
	cd $(PKG_BUILD_DIR); \
		CC="$(TARGET_CC)" \
		TARGET_OS="$(shell uname -s)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-DMKSH_SMALL=1 -DMKSH_ASSUME_UTF8=0" \
		HAVE_CAN_FSTACKPROTECTORALL=0 \
		LDFLAGS="$(TARGET_LDFLAGS)" \
			$(BASH) Build.sh -Q -r -c lto
endef

define Package/mksh/postinst
#!/bin/sh
grep mksh $${IPKG_INSTROOT}/etc/shells || \
	echo "/bin/mksh" >> $${IPKG_INSTROOT}/etc/shells
endef

define Package/mksh/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/dot.mkshrc $(1)/etc/mkshrc
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mksh $(1)/bin/
endef

define Package/mksh/conffiles
/etc/mkshrc
endef

$(eval $(call BuildPackage,mksh))
