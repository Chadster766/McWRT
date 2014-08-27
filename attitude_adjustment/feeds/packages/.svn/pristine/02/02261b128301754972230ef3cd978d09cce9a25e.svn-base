# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nmeap
PKG_VERSION:=0.3
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/nmeap
PKG_MD5SUM:=cbcc9fdf5176f9d6c9e48f27db443c4f

include $(INCLUDE_DIR)/package.mk

define Package/libnmeap
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=extensible NMEA-0183 parser written in standard C
  URL:=http://www.dmh2000.com/nmea/nmeap.shtml
endef

define Package/libnmeap/description
The NMEAP parser is intended for use in applications that need to input NMEA-0183 
	data from a GPS (typically over a serial port). NMEAP is a linkable library written in portable 
	standard C and is suitable for both desktop and embedded applications. 
	It is licensed under the BSD open source license.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_BUILD_DIR)/inc/nmeap{,_def}.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib/libnmeap.a $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libnmeap))
