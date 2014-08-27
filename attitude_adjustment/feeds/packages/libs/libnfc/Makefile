#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libnfc
PKG_VERSION:=1.4.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://libnfc.googlecode.com/files
PKG_MD5SUM:=c99b2b9212b1a1b064055a199779bdd8

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libnfc/Default
  TITLE:=Near Field Communication (NFC) library
  URL:=http://www.libnfc.org
endef

define Package/libnfc
$(call Package/libnfc/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libusb +pcscd +ccid
endef

define Package/libnfc/description
  libnfc is a free software library used to handle NFC capable devices.
  It supports the four main NFC modulations:
   * ISO14443-A (NXP MIFARE),
   * ISO14443-B,
   * JIS X 6319-4 (Sony Felica) and
   * Jewel Topaz (Innovision Research & Development).
  And depending on hardware, it supports:
   * Read (RFID @13.56Mhz),
   * Emulation and
   * Peer2Peer mode.  
endef

define Package/libnfc-bin
$(call Package/libnfc/Default)
  TITLE+= (binaries)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libnfc
endef

define Package/libnfc-bin/description
  libnfc is provided with some useful tools:
   * nfc-list: target listing;
   * nfc-mfclassic: Mifare Classic operations;
   * nfc-mfultralight: Mifare Ultralight operations.
endef

define Package/libnfc-examples
$(call Package/libnfc/Default)
  TITLE+= (examples)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libnfc
endef

define Package/libnfc-examples/description
  libnfc is provided with some examples like:
   * various targets emulation;
   * NFC frames relay (ie. over network);
   * manually done anticollision;
   * etc.
endef

TARGET_CFLAGS += $(FPIC)
CONFIGURE_ARGS += --without-readline

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/nfc $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnfc.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libnfc.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libnfc/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libnfc.so.* $(1)/usr/lib/
endef

define Package/libnfc-bin/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-list $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-mfclassic $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-mfultralight $(1)/usr/bin/
endef

define Package/libnfc-examples/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-anticol $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-dep-initiator $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-dep-target $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-emulate-forum-tag4 $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-emulate-tag $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-emulate-uid $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-poll $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-relay $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/nfc-relay-picc $(1)/usr/bin/
endef

$(eval $(call BuildPackage,libnfc))
$(eval $(call BuildPackage,libnfc-bin))
$(eval $(call BuildPackage,libnfc-examples))
