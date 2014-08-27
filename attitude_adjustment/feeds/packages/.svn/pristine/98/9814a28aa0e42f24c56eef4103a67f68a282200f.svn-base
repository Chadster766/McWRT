# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=glib
PKG_VERSION:=1.2.10
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=ftp://ftp.gtk.org/pub/gtk/v1.2 \
	http://ftp.gwdg.de/pub/misc/grafik/gimp/gtk/v1.2/ \
	http://ftp.esat.net/mirrors/ftp.gimp.org/pub/gtk/v1.2/ \
	http://www.mirrorservice.org/sites/ftp.gimp.org/pub/gtk/v1.2/
PKG_MD5SUM:=6fe30dad87c77b91b632def29dd69ef9

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=aclocal.m4 acinclude.m4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/glib1
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libpthread
  TITLE:=The GLib library of C routines
  URL:=http://www.gtk.org/
endef

define Package/glib1/description
 GLib is a library containing many useful C routines for things such 
 as trees, hashes, and lists.
endef

TARGET_CFLAGS += $(FPIC)
TARGET_LDFLAGS += -L$(TOOLCHAIN_DIR)/lib

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

CONFIGURE_VARS += \
	glib_cv_prog_cc_ansi_proto=no \
	glib_cv_has__inline=yes \
	glib_cv_has__inline__=yes \
	glib_cv_hasinline=yes \
	glib_cv_sane_realloc=yes \
	glib_cv_va_copy=no \
	glib_cv___va_copy=yes \
	glib_cv_va_val_copy=yes \
	glib_cv_rtldglobal_broken=no \
	glib_cv_uscore=no \
	glib_cv_func_pthread_mutex_trylock_posix=yes \
	glib_cv_func_pthread_cond_timedwait_posix=yes \
	glib_cv_sizeof_gmutex=24 \
	glib_cv_byte_contents_gmutex="0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0" \
	glib_cv_sys_pthread_getspecific_posix=yes \
	glib_cv_sys_pthread_mutex_trylock_posix=yes \
	glib_cv_sys_pthread_cond_timedwait_posix=yes \
	ac_cv_func_getpwuid_r=yes \
	ac_cv_func_getpwuid_r_posix=yes

define Build/InstallDev
	$(INSTALL_DIR) $(1) $(2)/bin
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	$(CP) $(PKG_BUILD_DIR)/.libs/*.a $(PKG_BUILD_DIR)/*/.libs/*.a $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/glibconfig-sysdefs.h $(1)/usr/include/glib-1.2/
	ln -sf $(STAGING_DIR)/usr/bin/glib-config $(2)/bin/
	$(SED) 's,-I$$$${prefix}/include/glib,-I$(STAGING_DIR)/usr/include/glib,g' $(1)/usr/bin/glib-config
	$(SED) 's,-I$$$${exec_prefix}/lib/glib,-I$(STAGING_DIR)/usr/lib/glib,g' $(1)/usr/bin/glib-config
	$(SED) 's,-L$$$${exec_prefix}/lib,,g' $(1)/usr/bin/glib-config
endef

define Package/glib1/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libg{lib,module,thread}-1.2.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,glib1))
