#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sipp
PKG_VERSION:=3.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME).$(PKG_VERSION).src.tar.gz
PKG_SOURCE_URL:=@SF/sipp
PKG_MD5SUM:=452a6f88f2d314dbb2f44e318a60982b

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/sipp
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Telephony
  DEPENDS:= +libncurses +libpthread $(CXX_DEPENDS)
  TITLE:=test tool / traffic generator for the SIP protocol
  URL:=http://sipp.sourceforge.net/
endef

define Package/sipp/description
 SIPp is a free Open Source test tool / traffic generator for the SIP
 protocol. It includes a few basic SipStone user agent scenarios (UAC and
 UAS) and establishes and releases multiple calls with the INVITE and BYE
 methods.
endef

TAR_CMD:=$(HOST_TAR) -C $(PKG_BUILD_DIR) --strip-components 1 $(TAR_OPTIONS)

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CC_linux="$(TARGET_CXX)" \
		CPP_linux="$(TARGET_CXX)" \
		CCLINK_linux="$(TARGET_CXX)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS_linux="$(TARGET_CPPFLAGS) -fno-rtti" \
		LFLAGS_linux="$(TARGET_LDFLAGS)" \
		LIBS="-lncurses -ldl -lpthread -lm" \
		all
endef

define Package/sipp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sipp $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,sipp))
