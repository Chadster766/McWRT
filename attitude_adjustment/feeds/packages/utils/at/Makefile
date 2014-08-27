#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=at
PKG_VERSION:=3.1.13
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/a/at
PKG_MD5SUM:=1da61af6c29e323abaaf13ee1a8dad79

include $(INCLUDE_DIR)/package.mk

define Package/at
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libelf
  TITLE:=Delayed job execution and batch processing
  URL:=http://packages.debian.org/stable/at
  MAINTAINER:=Luka Perkov <openwrt@lukaperkov.net>
endef

define Package/at/description
 At and batch read shell commands from standard input storing them as a job to
 be scheduled for execution in the future.
endef

export SENDMAIL=/bin/true
EXTRA_CFLAGS:=-DNEED_YYWRAP -I$(PKG_BUILD_DIR) -I$(STAGING_DIR)/usr/include/libelf \
	$(TARGET_LDFLAGS)

CONFIGURE_ARGS+=--prefix=/usr \
		--with-daemon_username=nobody \
		--with-daemon_groupname=nogroup \
		--with-jobdir=/var/spool/cron/atjobs \
		--with-atspool=/var/spool/cron/atspool

define Package/at/install
	$(INSTALL_DIR) $(1)/usr/bin $(1)/usr/sbin $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/atd.init $(1)/etc/init.d/atd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/at $(1)/usr/bin
	ln -sf at $(1)/usr/bin/atq
	ln -sf at $(1)/usr/bin/atrm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/atd $(1)/usr/sbin
endef

$(eval $(call BuildPackage,at))
