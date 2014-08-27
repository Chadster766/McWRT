#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nuttcp
PKG_VERSION:=6.1.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.lcp.nrl.navy.mil/pub/nuttcp
PKG_MD5SUM:=a16d4d6d5def02cea980e57feaf30500

include $(INCLUDE_DIR)/package.mk

define Package/nuttcp/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Network performance measurement tool
  URL:=http://lcp.nrl.navy.mil/nuttcp/nuttcp.html
endef

define Package/nuttcp/Default/description
  nuttcp is a network performance measurement tool intended for use by network
  and system managers. Its most basic usage is to determine the raw TCP (or UDP)
  network layer throughput by transferring memory buffers from a source system
  across an interconnecting network to a destination system, either transferring
  data for a specified time interval, or alternatively transferring a specified
  number of bytes. In addition to reporting the achieved network throughput in
  Mbps, nuttcp also provides additional useful information related to the data
  transfer such as user, system, and wall-clock time, transmitter and receiver
  CPU utilization, and loss percentage (for UDP transfers).
endef

define Package/nuttcp
  $(call Package/nuttcp/Default)
endef

define Package/nuttcp/description
  $(call Package/nuttcp/Default/description)
endef

define Package/nuttcp-xinetd
  $(call Package/nuttcp/Default)
  TITLE+= xinetd service files
  DEPENDS:=+nuttcp +xinetd
endef

define Package/nuttcp-xinetd/description
  $(call Package/nuttcp/Default/description)
  This package contains the nuttcp xinetd service files.
endef
  
MAKE_FLAGS += \
	APP="nuttcp" \
	OPT="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall"

define Package/nuttcp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nuttcp $(1)/usr/bin/
endef

define Package/nuttcp-xinetd/install
	$(INSTALL_DIR) $(1)/etc/xinetd.d
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/xinetd.d/nuttcp* $(1)/etc/xinetd.d/
	$(SED) 's,/local,,' $(1)/etc/xinetd.d/nuttcp*
endef

define Package/nuttcp-xinetd/conffiles
/etc/xinetd.d/nuttcp
/etc/xinetd.d/nuttcp4
/etc/xinetd.d/nuttcp6
endef

$(eval $(call BuildPackage,nuttcp))
$(eval $(call BuildPackage,nuttcp-xinetd))
