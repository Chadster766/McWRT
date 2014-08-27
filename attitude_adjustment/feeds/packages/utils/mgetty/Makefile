# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mgetty
PKG_VERSION:=1.1.36
PKG_XVERSION:=Jun15
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION)-$(PKG_XVERSION).tar.gz
PKG_SOURCE_URL:=ftp://alpha.greenie.net/pub/mgetty/source/1.1
PKG_MD5SUM:=0320e98c6b86bcca48fc5f355b94ead4

include $(INCLUDE_DIR)/package.mk

define Package/mgetty
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A data/fax solution for your analog modem
  URL:=http://mgetty.greenie.net/
endef

define Package/mgetty/description
A data/fax solution for your analog modem.
endef

define Build/Configure
	cp $(PKG_BUILD_DIR)/policy.h-dist $(PKG_BUILD_DIR)/policy.h
	echo "#define	AUTO_PPP" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	FAX_NOTIFY_PROGRAM" >> $(PKG_BUILD_DIR)/policy.h
	echo "#define	FAX_NOTIFY_PROGRAM \"/usr/lib/mgetty+sendfax/new_fax\"" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	DATA_FLOW" >> $(PKG_BUILD_DIR)/policy.h
	echo "#define	DATA_FLOW FLOW_HARD" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	DEVICE_GROUP" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	DEVICE_OWNER" >> $(PKG_BUILD_DIR)/policy.h
	echo "#define	DEVICE_OWNER \"root\"" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	FAXREC_FLOW" >> $(PKG_BUILD_DIR)/policy.h
	echo "#define	FAXREC_FLOW FLOW_SOFT" >> $(PKG_BUILD_DIR)/policy.h
	echo "#undef	FAXSEND_FLOW" >> $(PKG_BUILD_DIR)/policy.h
	echo "#define	FAXSEND_FLOW FLOW_SOFT" >> $(PKG_BUILD_DIR)/policy.h
endef

#FIXME: don't manually install, use upstream Makefile instead
define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	mkdir -p $(PKG_INSTALL_DIR)/usr/sbin
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax
	mkdir -p $(PKG_INSTALL_DIR)/etc/mgetty+sendfax
	$(MAKE) -C $(PKG_BUILD_DIR) \
		mksed
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		prefix="$(PKG_INSTALL_DIR)/usr" \
		CONFDIR=/etc/mgetty+sendfax \
		bin-all
	$(CP) $(PKG_BUILD_DIR)/mgetty $(PKG_INSTALL_DIR)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/sendfax $(PKG_INSTALL_DIR)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/fax/faxrunqd $(PKG_INSTALL_DIR)/usr/sbin
	chmod 755 $(PKG_INSTALL_DIR)/usr/sbin/*
	$(CP) $(PKG_BUILD_DIR)/fax/faxq-helper $(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax
	chmod u+s $(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax/faxq-helper
	$(CP) $(PKG_BUILD_DIR)/fax/cour25n.pbm $(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax
	$(CP) $(PKG_BUILD_DIR)/fax/cour25.pbm $(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax
	$(CP) $(PKG_BUILD_DIR)/fax/faxq $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/fax/faxrm $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/fax/faxrunq $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/fax/faxspool $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/g3/g32pbm $(PKG_INSTALL_DIR)/usr/bin
	(cd $(PKG_INSTALL_DIR)/usr/bin;\
		ln -snf g32pbm g3topbm;\
	);
	$(CP) $(PKG_BUILD_DIR)/g3/g3cat $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/g3/pbm2g3 $(PKG_INSTALL_DIR)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/g3/sff2g3 $(PKG_INSTALL_DIR)/usr/bin
	chmod 755 $(PKG_INSTALL_DIR)/usr/bin/*
# Installation into /etc
	$(CP) $(PKG_BUILD_DIR)/dialin.config $(PKG_INSTALL_DIR)/etc/mgetty+sendfax
	$(CP) $(PKG_BUILD_DIR)/fax/faxheader $(PKG_INSTALL_DIR)/etc/mgetty+sendfax
	$(CP) $(PKG_BUILD_DIR)/faxrunq.config $(PKG_INSTALL_DIR)/etc/mgetty+sendfax
	$(CP) $(PKG_BUILD_DIR)/fax/faxspool.rules $(PKG_INSTALL_DIR)/etc/mgetty+sendfax/faxspool.rules.sample
	$(PKG_BUILD_DIR)/sedscript < $(PKG_BUILD_DIR)/login.cfg.in > $(PKG_INSTALL_DIR)/etc/mgetty+sendfax/login.config   
	$(PKG_BUILD_DIR)/sedscript < $(PKG_BUILD_DIR)/mgetty.cfg.in > $(PKG_INSTALL_DIR)/etc/mgetty+sendfax/mgetty.config   
	$(PKG_BUILD_DIR)/sedscript < $(PKG_BUILD_DIR)/sendfax.cfg.in > $(PKG_INSTALL_DIR)/etc/mgetty+sendfax/sendfax.config   
endef

define Package/mgetty/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/bin/faxq \
		$(PKG_INSTALL_DIR)/usr/bin/faxrm \
		$(PKG_INSTALL_DIR)/usr/bin/faxrunq \
		$(PKG_INSTALL_DIR)/usr/bin/faxspool \
		$(PKG_INSTALL_DIR)/usr/bin/g32pbm \
		$(PKG_INSTALL_DIR)/usr/bin/g3cat \
		$(PKG_INSTALL_DIR)/usr/bin/pbm2g3 \
		$(PKG_INSTALL_DIR)/usr/bin/sff2g3 \
		$(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(PKG_INSTALL_DIR)/usr/sbin/mgetty \
		$(PKG_INSTALL_DIR)/usr/sbin/sendfax \
		$(PKG_INSTALL_DIR)/usr/sbin/faxrunqd \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/mgetty+sendfax
	install -m4755 \
		$(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax/faxq-helper \
		$(1)/usr/lib/mgetty+sendfax/
	install -m644 \
		$(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax/cour25n.pbm \
		$(PKG_INSTALL_DIR)/usr/lib/mgetty+sendfax/cour25.pbm \
		$(1)/usr/lib/mgetty+sendfax/
	$(INSTALL_DIR) $(1)/etc/mgetty+sendfax
	install -m600 \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/dialin.config \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/faxheader \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/faxrunq.config \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/faxspool.rules.sample \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/login.config \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/mgetty.config \
		$(PKG_INSTALL_DIR)/etc/mgetty+sendfax/sendfax.config \
		$(1)/etc/mgetty+sendfax/
endef

$(eval $(call BuildPackage,mgetty))
