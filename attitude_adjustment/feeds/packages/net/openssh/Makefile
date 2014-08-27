#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=openssh
PKG_VERSION:=6.1p1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.openbsd.org/pub/OpenBSD/OpenSSH/portable/ \
		ftp://openbsd.wiretapped.net/pub/OpenBSD/OpenSSH/portable/ \
		ftp://ftp.belnet.be/packages/openbsd/OpenSSH/portable/ \
		ftp://ftp.de.openbsd.org/pub/unix/OpenBSD/OpenSSH/portable/
PKG_MD5SUM:=3345cbf4efe90ffb06a78670ab2d05d5

PKG_BUILD_DEPENDS:=libopenssl

include $(INCLUDE_DIR)/package.mk

define Package/openssh/Default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl +zlib +SSP_SUPPORT:libssp
  TITLE:=OpenSSH
  MAINTAINER:=Peter Wagner <tripolar@gmx.at>
  URL:=http://www.openssh.com/
  SUBMENU:=SSH
endef

define Package/openssh-moduli
  $(call Package/openssh/Default)
  DEPENDS+= +openssh-server
  TITLE+= moduli file
endef

define Package/openssh-moduli/description
OpenSSH server moduli file.
endef


define Package/openssh-client
  $(call Package/openssh/Default)
  TITLE+= client
endef

define Package/openssh-client/description
OpenSSH client.
endef

define Package/openssh-client/conffiles
/etc/ssh/ssh_config
endef

define Package/openssh-client-utils
  $(call Package/openssh/Default)
  DEPENDS+= +openssh-client +openssh-keygen
  TITLE+= client utilities
endef

define Package/openssh-client-utils/description
OpenSSH client utilities.
endef

define Package/openssh-keygen
  $(call Package/openssh/Default)
  TITLE+= keygen
endef

define Package/openssh-keygen/description
OpenSSH keygen.
endef

define Package/openssh-server
  $(call Package/openssh/Default)
  DEPENDS+= +openssh-keygen
  TITLE+= server
endef

define Package/openssh-server/description
OpenSSH server.
endef

define Package/openssh-server/conffiles
/etc/ssh/sshd_config
endef

define Package/openssh-sftp-client
  $(call Package/openssh/Default)
  TITLE+= SFTP client
endef

define Package/openssh-sftp-client/description
OpenSSH SFTP client.
endef

define Package/openssh-sftp-server
  $(call Package/openssh/Default)
  TITLE+= SFTP server
endef

define Package/openssh-sftp-server/description
OpenSSH SFTP server.
endef

CONFIGURE_ARGS+= \
	$(DISABLE_NLS) \
	--sysconfdir=/etc/ssh \
	--with-privsep-user=sshd \
	--with-privsep-path=/var/empty \
	--enable-shared \
	--disable-static \
	--disable-debug \
	--disable-strip \
	--disable-etc-default-login \
	--disable-lastlog \
	--disable-utmp \
	--disable-utmpx \
	--disable-wtmp \
	--disable-wtmpx \
	--without-bsd-auth \
	--without-kerberos5 \
	--without-pam \
	--without-x

ifeq ($(CONFIG_OPENSSL_ENGINE),y)
CONFIGURE_ARGS+= \
	--with-ssl-engine
endif

ifneq ($(CONFIG_SSP_SUPPORT),y)
CONFIGURE_ARGS += \
	--without-stackprotect
endif

CONFIGURE_VARS += LD="$(TARGET_CC)"

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		LIBS="" \
		sftp-server
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		STRIP_OPT="" \
		all install
endef

define Package/openssh-moduli/install
	$(INSTALL_DIR) $(1)/etc/ssh
	$(CP) $(PKG_INSTALL_DIR)/etc/ssh/moduli $(1)/etc/ssh/
endef

define Package/openssh-client/install
	$(INSTALL_DIR) $(1)/etc/ssh
	chmod 0700 $(1)/etc/ssh
	$(CP) $(PKG_INSTALL_DIR)/etc/ssh/ssh_config $(1)/etc/ssh/
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ssh $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/scp $(1)/usr/bin/
endef

define Package/openssh-client-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(foreach bin,add agent keyscan keysign,$(PKG_BUILD_DIR)/ssh-$(bin)) $(1)/usr/bin/
endef

define Package/openssh-keygen/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/ssh-keygen $(1)/usr/bin/
endef


define Package/openssh-server/install
	$(INSTALL_DIR) $(1)/etc/ssh
	chmod 0700 $(1)/etc/ssh
	$(CP) $(PKG_INSTALL_DIR)/etc/ssh/sshd_config $(1)/etc/ssh/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/sshd.init $(1)/etc/init.d/sshd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/sshd $(1)/usr/sbin/
endef

define Package/openssh-sftp-client/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/sftp $(1)/usr/bin/
endef

define Package/openssh-sftp-server/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sftp-server $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/libexec
	ln -sf ../lib/sftp-server $(1)/usr/libexec/sftp-server
endef

$(eval $(call BuildPackage,openssh-client))
$(eval $(call BuildPackage,openssh-moduli))
$(eval $(call BuildPackage,openssh-client-utils))
$(eval $(call BuildPackage,openssh-keygen))
$(eval $(call BuildPackage,openssh-server))
$(eval $(call BuildPackage,openssh-sftp-client))
$(eval $(call BuildPackage,openssh-sftp-server))
