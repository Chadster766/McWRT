#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bitlbee
PKG_VERSION:=1.0.1
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://get.bitlbee.org/src \
	http://get.bitlbee.be/src \
	http://get.us.bitlbee.org/src \
	http://ftp.snt.utwente.nl/pub/software/bitlbee/src
PKG_MD5SUM:=a4d7daa9b64898dd41a3a6d104692d9a

PKG_BUILD_DEPENDS:=libopenssl glib1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/bitlbee
  SUBMENU:=Instant Messaging
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl +glib1 $(ICONV_DEPENDS)
  TITLE:=IRC gateway to IM chat networks
  URL:=http://www.bitlbee.org/
endef

define Package/bitlbee/conffiles
/etc/bitlbee/motd.txt
/etc/bitlbee/bitlbee.conf
endef

define Build/Configure
	# it's not GNU autoconf stuff
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		STAGING_DIR="$(STAGING_DIR)" \
		STRIP="$(STRIP)" \
		./configure \
			--prefix=/usr/ \
			--bindir=/usr/sbin/ \
			--etcdir=/etc/bitlbee/ \
			--datadir=/usr/share/bitlbee/ \
			--config=/var/lib/bitlbee/ \
			--arch=OpenWrt \
			--cpu="$(ARCH)" \
			--debug=0 \
			--strip=1 \
			--sizeopt=1 \
			--ipv6=1 \
			--ssl=openssl \
			--glib1=1 \
			--glib1prefix="$(STAGING_DIR)/usr" \
	);
	$(SED) 's,^CFLAGS=.*,CFLAGS=$(TARGET_CFLAGS),g'\
		$(PKG_BUILD_DIR)/Makefile.settings
	@echo 'CFLAGS+=-I$(ICONV_PREFIX)/include' >> \
		$(PKG_BUILD_DIR)/Makefile.settings
	@echo 'LFLAGS+=$(TARGET_LDFLAGS)' >> \
		$(PKG_BUILD_DIR)/Makefile.settings
	@echo 'LFLAGS+=-L$(ICONV_PREFIX)/lib' >> \
		$(PKG_BUILD_DIR)/Makefile.settings
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/bitlbee/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(1)" install-etc install-bin 
	$(MAKE) -C $(PKG_BUILD_DIR)/doc/user-guide \
		DESTDIR="$(1)" install 
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/bitlbee.init $(1)/etc/init.d/bitlbee
endef

$(eval $(call BuildPackage,bitlbee))
