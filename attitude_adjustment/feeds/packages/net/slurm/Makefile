#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=slurm
PKG_VERSION:=0.3.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.wormulon.net/slurm/
PKG_MD5SUM:=e68d09202b835c644f7f6b7f070f29a2

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/slurm
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libncurses
  TITLE:=A realtime network interface monitor
  URL:=http://www.wormulon.net/slurm/
endef

define Package/slurm/description
	A realtime network interface monitor with the following features: 
	- realtime traffic statistics divided into incoming and outgoing ;
	- optional combined view ;
	- can monitor any kind of network interface ;
	- shows detailed statistics about the interface ;
	- it's themeable.
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); touch \
		configure.in \
		aclocal.m4 \
		Makefile.in \
		config.h.in \
		configure \
	);
	$(call Build/Configure/Default, \
		, \
		LIBS="-lncurses" \
		ac_cv_lib_ncurses_use_default_colors=yes \
	)
endef

define Package/slurm/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/slurm $(1)/usr/bin/
endef

$(eval $(call BuildPackage,slurm))
