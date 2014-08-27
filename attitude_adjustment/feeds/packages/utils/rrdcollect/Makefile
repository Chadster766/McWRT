#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rrdcollect
PKG_VERSION:=0.2.4
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/rrdcollect
PKG_MD5SUM:=fd7ac95195e3e5cbab0677629505d9be

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/rrdcollect/Default
  SECTION:=utils
  CATEGORY:=Utilities
  URL:=http://rrdcollect.sourceforge.net/
endef

define Package/rrdcollect/Default/description
 RRDcollect is a daemon which polls ceratin files in /proc/ 
 directory, gathering data and storing it inside RRDtool's 
 database files. Being written in C should be both fast 
 and resources-friendly. Supports both scanf(3)-style 
 pattern matches and perl compatible regular expressions.	
endef

define Package/rrdcollect10
$(call Package/rrdcollect/Default)
  TITLE:=Round-Robin Database (RRD) collecting daemon
  PROVIDES:=rrdcollect
  DEPENDS:=+librrd1
  VARIANT:=rrdtool10
endef

define Package/rrdcollect10/description
$(call Package/rrdcollect/Default/description)
 .
 This package contains the RRD collecting daemon, linked against 
 rrdtool-1.0.x shared library.
endef

define Package/rrdcollect12
$(call Package/rrdcollect/Default)
  TITLE:=Round-Robin Database (RRD) collecting daemon
  PROVIDES:=rrdcollect
  DEPENDS:=+librrd
  VARIANT:=rrdtool12
endef

define Package/rrdcollect12/description
$(call Package/rrdcollect/Default/description)
 .
 This package contains the RRD collecting daemon, linked against 
 rrdtool-1.2.x shared library.
endef


define Package/rrdcollect10-example
$(call Package/rrdcollect/Default)
  TITLE:=Example setup for RRD collecting daemon above
  DEPENDS:=rrdcollect +rrdtool1
  VARIANT:=rrdtool12
endef

define Package/rrdcollect10-example/description
$(call Package/rrdcollect/Default/description)
 .
 This package contains examples for the RRD collecting daemon, using
 rrdtool-1.0.x shared library and utilities.
endef

define Package/rrdcollect12-example
$(call Package/rrdcollect/Default)
  TITLE:=Example setup for RRD collecting daemon above
  DEPENDS:=rrdcollect +rrdtool
  VARIANT:=rrdtool12
endef

define Package/rrdcollect12-example/description
$(call Package/rrdcollect/Default/description)
 This package contains examples for the RRD collecting daemon, using
 rrdtool-1.2.x shared library and utilities.
endef


CONFIGURE_ARGS+= \
	--enable-shared \
	--disable-static \
	--disable-rpath \
	--enable-exec \
	--without-rrdtool \
	--with-librrd \
	--without-libpcre \
	--without-libpcap \

CONFIGURE_VARS+= \
	CFLAGS="$(TARGET_CFLAGS) -DSOCKET_COMM" \

EXTRA_LDFLAGS+= -Wl,-rpath-link,$(STAGING_DIR)/usr/lib

ifeq ($(BUILD_VARIANT),rrdtool10)

  TARGET_CPPFLAGS:=-I$(STAGING_DIR)/usr/lib/rrdtool-1.0/include $(TARGET_CPPFLAGS)
  TARGET_LDFLAGS:=-L$(STAGING_DIR)/usr/lib/rrdtool-1.0/lib $(TARGET_CPPFLAGS)

endif

ifeq ($(BUILD_VARIANT),rrdtool12)

  TARGET_CPPFLAGS:=-I$(STAGING_DIR)/usr/lib/rrdtool-1.2/include $(TARGET_CPPFLAGS)
  TARGET_LDFLAGS:=-L$(STAGING_DIR)/usr/lib/rrdtool-1.2/lib $(TARGET_CPPFLAGS)

endif

define Package/rrdcollect/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/rrdcollect $(1)/usr/sbin/
endef

Package/rrdcollect10/install = $(Package/rrdcollect/install)
Package/rrdcollect12/install = $(Package/rrdcollect/install)

define Package/rrdcollect-example/conffiles
/etc/rrd.conf
/etc/rrdcollect.conf
endef

define Package/rrdcollect-example/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/rrd.conf $(1)/etc/
	$(INSTALL_DATA) ./files/rrdcollect.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/rrdcollect.init $(1)/etc/init.d/rrdcollect
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/rrd.sh $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/www/cgi-bin
	ln -sf /var/lib/rrdcollect/rrd.cgi $(1)/www/cgi-bin/rrd.cgi
	ln -sf /var/lib/rrdcollect/img $(1)/www/img
endef

Package/rrdcollect10-example/install = $(Package/rrdcollect-example/install)
Package/rrdcollect12-example/install = $(Package/rrdcollect-example/install)

$(eval $(call BuildPackage,rrdcollect10))
$(eval $(call BuildPackage,rrdcollect10-example))
$(eval $(call BuildPackage,rrdcollect12))
$(eval $(call BuildPackage,rrdcollect12-example))
