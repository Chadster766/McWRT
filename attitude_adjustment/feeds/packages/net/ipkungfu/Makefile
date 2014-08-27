#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipkungfu
PKG_VERSION:=0.6.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.linuxkungfu.org/ipkungfu
PKG_MD5SUM:=89f6bc0df356dd4ee650882c54ec9074

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/ipkungfu
  SECTION:=net
  CATEGORY:=Network
  TITLE:=IPKungFu is an iptables-based Linux firewall
  URL:=http://www.linuxkungfu.org/
  SUBMENU:=Firewall
  DEPENDS:=+bash
endef

define Package/ipkungfu/description
	IPKungFu is an iptables-based Linux firewall.
	It aims to simplify the configuration of Internet
	connection sharing, port forwarding, and packet filtering.
endef

define Package/ipkungfu/conffiles
/etc/ipkungfu/accept_hosts.conf
/etc/ipkungfu/custom.conf
/etc/ipkungfu/forward.conf
/etc/ipkungfu/log.conf
/etc/ipkungfu/pre.conf
/etc/ipkungfu/services.conf
/etc/ipkungfu/advanced.conf
/etc/ipkungfu/deny_hosts.conf
/etc/ipkungfu/ipkungfu.conf
/etc/ipkungfu/post.conf
/etc/ipkungfu/redirect.conf
/etc/ipkungfu/vhosts.conf
endef

define Package/ipkungfu/postrm
#!/bin/sh
rm -rf /etc/ipkungfu/cache
rmdir /etc/ipkungfu 2>/dev/null || true
endef

# uses GNU configure
CONFIGURE_VARS+= \
		 ac_cv_path_IPTABLES="/usr/sbin/iptables" \
		 ac_cv_path_MD5SUM="/usr/bin/md5sum" \
		 ac_cv_path_MODPROBE="/sbin/insmod" \
		 ac_cv_path_RMMOD="/sbin/rmmode" \
		 ac_cv_path_DEPMOD="/bin/true" \
		 ac_cv_path_LSMOD="/sbin/lsmod" \
		 ac_cv_path_CUT="/usr/bincut" \
		 ac_cv_path_TR="/usr/bintr" \
		 ac_cv_path_GREP="/bin/grep" \
		 ac_cv_path_SED="/bin/sed" \
		 ac_cv_path_AWK="/usr/bin/awk" \

define Package/ipkungfu/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dummy_server $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkungfu $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/ipkungfu/
	$(CP) $(PKG_BUILD_DIR)/files/conf/* $(1)/etc/ipkungfu/
endef

$(eval $(call BuildPackage,ipkungfu))
