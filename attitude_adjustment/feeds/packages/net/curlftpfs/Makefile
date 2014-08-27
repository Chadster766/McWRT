# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=curlftpfs
PKG_VERSION:=0.9.2
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/curlftpfs
PKG_MD5SUM:=b452123f755114cd4461d56c648d9f12

PKG_BUILD_DEPENDS:=libiconv
PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/curlftpfs
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Filesystem
  DEPENDS:=+libcurl +libfuse +glib2
  TITLE:=CurlFtpFS
  URL:=http://curlftpfs.sourceforge.net/
endef

define Package/curlftpfs/description
  CurlFtpFS is a filesystem for accessing FTP hosts
  based on FUSE and libcurl.
endef

EXTRA_CFLAGS += \
	-I$(ICONV_PREFIX)/include \
	-I$(STAGING_DIR)/usr/include/fuse \
	-DICONV_CONST=const

EXTRA_LDFLAGS += \
	-L$(INTL_PREFIX)/lib \
	-L$(ICONV_PREFIX)/lib \
	-lpthread

define Package/curlftpfs/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/curlftpfs $(1)/usr/bin/
endef

$(eval $(call BuildPackage,curlftpfs))

