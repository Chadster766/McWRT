#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fping
PKG_VERSION:=2.4b2_to-ipv6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.fping.com/download/
PKG_MD5SUM:=3ad516765514249a40d3c5b6caab812a

include $(INCLUDE_DIR)/package.mk

define Package/fping
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A program to ping multiple hosts in parallel
  URL:=http://www.fping.com
endef

define Package/fping/description
	fping is a ping(1) like program which uses the Internet Control Message Protocol 
	(ICMP) echo request to determine if a host is up. fping is different from ping in 
	that you can specify any number of hosts on the command line, or specify a file 
	containing the lists of hosts to ping. Instead of trying one host until it timeouts 
	or replies, fping will send out a ping packet and move on to the next host in a 
	round-robin fashion. If a host replies, it is noted and removed from the list of 
	hosts to check. If a host does not respond within a certain time limit and/or retry 
	limit it will be considered unreachable.
endef
  
define Build/Configure
        (cd $(PKG_BUILD_DIR); touch \
		configure \
		stamp-h.in \
		config.h.in \
	);
	$(call Build/Configure/Default,)
endef

define Build/Compile	
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) clean $(PKG_NAME)
	mv $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME)4
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS) -DIPV6=1" -C $(PKG_BUILD_DIR) clean $(PKG_NAME)
	mv $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME)6
endef

define Package/fping/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/fping4 $(1)/usr/bin/
	$(CP) $(PKG_BUILD_DIR)/fping6 $(1)/usr/bin/
	ln -sf fping4 $(1)/usr/bin/fping
endef

$(eval $(call BuildPackage,fping))
