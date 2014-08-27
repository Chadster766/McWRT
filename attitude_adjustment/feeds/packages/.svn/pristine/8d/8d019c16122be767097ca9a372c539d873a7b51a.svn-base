#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mysql
PKG_VERSION:=5.1.53
PKG_RELEASE:=7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=\
	http://mysql.planetmirror.com/Downloads/MySQL-5.1/ \
	http://mysql.he.net/Downloads/MySQL-5.1/ \
	http://www.linorg.usp.br/mysql/Downloads/MySQL-5.1/ \
	http://mysql.holywar.net/Downloads/MySQL-5.1/
PKG_MD5SUM:=e5e7c5b0b5c382489e6a66778703bc46

PKG_BUILD_DEPENDS:=libncurses libreadline
PKG_BUILD_PARALLEL:=1

PKG_FIXUP:=libtool no-autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/libmysqlclient/Default
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+uclibcxx +zlib
  TITLE:=MySQL client library
  URL:=http://dev.mysql.com/
endef

define Package/mysql-server/Default
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libmysqlclient +libpthread +libncursesw +libreadline
  TITLE:=MySQL Server
  URL:=http://dev.mysql.com/
  SUBMENU:=database
endef

define Package/mysql-server
  $(call Package/mysql-server/Default)
endef

define Package/libmysqlclient
  $(call Package/libmysqlclient/Default)
endef

define Package/libmysqlclient-r
  $(call Package/libmysqlclient/Default)
  TITLE += threadsafe
  DEPENDS+= +libpthread
endef

TARGET_CXX=g++-uc

CONFIGURE_VARS += \
        CXXFLAGS="$$$$CXXFLAGS -fno-builtin -fno-rtti -nostdinc++"  \
        CPPFLAGS="$$$$CPPFLAGS -I$(STAGING_DIR)/usr/include/uClibc++" \
        LDFLAGS="$$$$LDFLAGS" \
        LIBS="-nodefaultlibs -luClibc++ -lm -lc $(LIBGCC_S)" \
        CLIENTCLIBS="-nodefaultlibs -luClibc++ -lm -lc $(LIBGCC_S)" \


define Build/Configure
	(cd $(PKG_BUILD_DIR); touch \
		configure.in \
		aclocal.m4 \
		Makefile.in \
		config.h.in \
		configure \
	);
	$(CP) files/lex_hash.h $(PKG_BUILD_DIR)/sql/lex_hash.h

	$(call Build/Configure/Default, \
		--enable-shared \
		--enable-static \
		--enable-thread-safe-client \
		--disable-assembler \
		--with-pthread \
		--with-server \
		--without-mysqlmanager \
		--without-raid \
		--with-mysqld-user=root \
		--with-unix-socket-path=/tmp \
		--without-libwrap \
		--without-pstack \
		--with-low-memory \
		--without-embedded-server \
		--without-query-cache \
		--without-mysqlfs \
		--without-vio \
		--without-ssl \
		--without-docs \
		--without-man \
		--without-bench \
		--without-readline \
		--without-debug \
		--with-named-thread-libs=-lpthread \
		, \
		mysql_cv_compress=yes \
		mysql_cv_gethostname_style=glibc2 \
		ac_cv_c_stack_direction=-1 \
	);
endef

TARGET_CFLAGS += $(FPIC)

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="include" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	+$(MAKE) $(PKG_JOBS) -C "$(PKG_BUILD_DIR)/libmysql" \
		CC="g++" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		conf_to_src
	+$(MAKE) $(PKG_JOBS) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="libmysql" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="libmysql" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	+$(MAKE) $(PKG_JOBS) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="libmysql_r" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="libmysql_r" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		SUBDIRS="scripts" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		bin_SCRIPTS="mysql_config" \
		install
	+$(MAKE) $(PKG_JOBS) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef


define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin $(1)/usr/bin $(1)/usr/include $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mysql_config $(1)/usr/bin/
	ln -sf $(STAGING_DIR)/usr/bin/mysql_config $(2)/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/mysql $(1)/usr/include/
	# NOTE: needed for MySQL-Python
	$(CP) $(PKG_BUILD_DIR)/include/mysqld_error.h $(1)/usr/include/mysql/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/mysql $(1)/usr/lib/
	rm -f $(1)/usr/lib/mysql/libmysqlclient.la
endef

define Package/libmysqlclient/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/mysql/libmysqlclient.so.* $(1)/usr/lib/
endef
define Package/libmysqlclient-r/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/mysql/libmysqlclient_r.so.* $(1)/usr/lib/
endef

define Package/mysql-server/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mysql $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/mysqld $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/myisamchk $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mysqladmin $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mysqldump $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/mysql_install_db $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/my_print_defaults $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) files/mysqld.init $(1)/etc/init.d/mysqld
	$(INSTALL_CONF) conf/my.cnf $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/share/mysql
	$(INSTALL_DIR) $(1)/usr/share/mysql/english
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/mysql/english/errmsg.sys $(1)/usr/share/mysql/english
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/mysql/fill_help_tables.sql $(1)/usr/share/mysql/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/mysql/mysql_system_tables.sql $(1)/usr/share/mysql/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/share/mysql/mysql_system_tables_data.sql $(1)/usr/share/mysql/
endef

define Package/mysql-server/conffiles
/etc/my.cnf
endef

$(eval $(call BuildPackage,mysql-server))
$(eval $(call BuildPackage,libmysqlclient))
$(eval $(call BuildPackage,libmysqlclient-r))
