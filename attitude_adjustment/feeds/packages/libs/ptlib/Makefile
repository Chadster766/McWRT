# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=ptlib
PKG_VERSION:=2.2.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-src.tar.bz2
PKG_SOURCE_URL:=@SF/opalvoip
PKG_MD5SUM:=dc266ba315e130b304854592e6388725

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/ptlib
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libexpat
  TITLE:=Portable Tools library
  URL:=http://www.opalvoip.org/
endef

define Package/ptlib/description
 PTLib is a moderately large class library that has its genesis many years ago as
 PWLib (portable Windows Library), a method to product applications to run on both
 Microsoft Windows and Unix X-Windows systems. It also was to have a Macintosh port
 as well but that never eventuated. The parts of the library relating to GUI
 functions have long been removed as other class libraries such as wxWidgets were
 much more advanced.
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-minsize \
	--enable-opal \
	--enable-internalregex \
	--enable-audio \
	--disable-alsa \
	--disable-asn \
	--disable-avc \
	--enable-configfile \
	--disable-dc \
	--disable-dtmf \
	--enable-expat \
	--disable-ftp \
	--disable-http \
	--disable-httpsvc \
	--disable-jabber \
	--disable-odbc \
	--disable-openldap \
	--disable-openssl \
	--disable-oss \
	--disable-pipechan \
	--disable-plugins \
	--disable-pop3smtp \
	--disable-remconn \
	--enable-resolver \
	--enable-qos \
	--disable-sasl \
	--disable-sdl \
	--disable-serial \
	--disable-shm-video \
	--disable-snmp \
	--disable-soap \
	--disable-socks \
	--enable-stun \
	--disable-telnet \
	--disable-tts \
	--disable-v4l \
	--disable-v4l2 \
	--disable-bsdvideo \
	--disable-video \
	--disable-vxml \
	--enable-wavfile \
	--disable-xmlrpc \

CONFIGURE_VARS += \
	CXXFLAGS="$(TARGET_CFLAGS) -fno-builtin -fno-rtti" \

CONFIGURE_CMD = ./configure

define Build/Configure/Default
        (cd $(PKG_BUILD_DIR)/$(CONFIGURE_PATH)/$(strip $(3)); \
        if [ -x $(CONFIGURE_CMD) ]; then \
                $(CP) $(SCRIPT_DIR)/config.{guess,sub} $(PKG_BUILD_DIR)/$(3)/ && \
                $(CONFIGURE_VARS) \
                $(2) \
                $(CONFIGURE_CMD) \
                $(CONFIGURE_ARGS) \
                $(1); \
        fi; \
        )
endef

MAKE_FLAGS += optnoshared

define Build/InstallDev
	ln -sf $(PKG_NAME)_$(PKG_VERSION) $(BUILD_DIR)/$(PKG_NAME)
	$(SED) 's|//\(#include <ptlib/sound.h>\)|\1|' $(PKG_BUILD_DIR)/include/ptlib.h
endef

$(eval $(call BuildPackage,ptlib))
