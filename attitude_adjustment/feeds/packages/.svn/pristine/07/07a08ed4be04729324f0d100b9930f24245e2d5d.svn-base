# 
# Copyright (C) 2008-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=imsnif
PKG_VERSION:=0.04
PKG_RELEASE:=4

PKG_SOURCE_URL:=@SF/im-snif
PKG_SOURCE:=$(PKG_NAME)f_$(PKG_VERSION).tgz
PKG_MD5SUM:=689c456f8edb77b9b4199a25514ac683

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

PKG_UNPACK:=gzip -dc $(DL_DIR)/$(PKG_SOURCE) | $(TAR) -C $(PKG_BUILD_DIR) -xvf -

define Package/imsnif
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Instant Messaging
  DEPENDS:=+libpcap $(CXX_DEPENDS)
  TITLE:=MSN Messenger sniffer
  URL:=http://sourceforge.net/projects/im-snif
endef

define Package/imsnif/description
  IMsnif is a simple program to log Instant Message activity
  on the network. It uses libpcap to capture packets and
  analyzes them, logging conversation, contact lists, etc.
endef

define Build/Compile
	$(TARGET_CXX) $(TARGET_CPPFLAGS) -fno-rtti -I$(LINUX_DIR)/include \
		-o $(PKG_BUILD_DIR)/src/imsnif \
			$(PKG_BUILD_DIR)/src/imsniff.cpp \
			$(PKG_BUILD_DIR)/src/util.cpp \
			$(PKG_BUILD_DIR)/src/msn_conntrack.cpp \
			$(PKG_BUILD_DIR)/src/msn_handlers.cpp \
		-L$(STAGING_DIR)/usr/lib -lpcap \
			$(PKG_BUILD_DIR)/src/pcap_stuff.cpp
endef

define Package/imsnif/install
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/docs/imsniff.conf.sample $(1)/etc/imsnif.conf
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/imsnif $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/imsnif $(1)/usr/bin
endef

define Package/imsnif/conffiles
/etc/imsnif.conf
endef

$(eval $(call BuildPackage,imsnif))
