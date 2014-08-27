#
# Copyright (C) 2008-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=gstreamer
PKG_VERSION:=0.10.34
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://gstreamer.freedesktop.org/src/gstreamer/
PKG_MD5SUM:=e6938af242cd002be9f0b6441e848d83

PKG_FIXUP:=autoreconf
PKG_REMOVE_FILES:=autogen.sh aclocal.m4

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/nls.mk

define Package/gstreamer/Default
  CATEGORY:=Multimedia
  SECTION:=multimedia
  TITLE:=GStreamer
  URL:=http://gstreamer.freedesktop.org/
endef

define Package/gstreamer/description/Default
 GStreamer open source multimedia framework
endef


define Package/gstreamer
$(call Package/gstreamer/Default)
  TITLE+= framework
  DEPENDS+= +libgstreamer \
	+libgstcheck \
	+libgstcontroller \
	+libgstdataprotocol \
	+libgstnet
endef

define Package/gstreamer/description
$(call Package/gstreamer/description/Default)
 .
 This meta package contains only dependencies on the other GStreamer
 componenents.
endef


define Package/gstreamer-utils
$(call Package/gstreamer/Default)
  TITLE+= utilities
  DEPENDS+= +libgstreamer
endef

define Package/gstreamer-utils/description
$(call Package/gstreamer/description/Default)
 .
 This package contains the GStreamer utilities.
endef


define Package/libgstreamer
$(call Package/gstreamer/Default)
  TITLE+= library (core)
  DEPENDS+= +glib2 +libpthread +libxml2
endef

define Package/libgstreamer/description
$(call Package/gstreamer/description/Default)
 .
 This package contains the GStreamer core library.
endef
  

GST_VERSION:=0.10

CONFIGURE_ARGS += \
	--disable-debug \
	--disable-examples \
	--disable-tests \
	--disable-valgrind \
	\
	--disable-gst-debug \
	\
	--without-libiconv-prefix \
	--without-libintl-prefix \
	--without-x \

EXTRA_LDFLAGS+= \
	-Wl,-rpath-link=$(STAGING_DIR)/usr/lib \
	-L$(ICONV_PREFIX)/lib -L$(INTL_PREFIX)/lib

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/include/gstreamer-$(GST_VERSION)/* \
		$(1)/usr/include/gstreamer-$(GST_VERSION)/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/libgst*-$(GST_VERSION).{a,la,so*} \
		$(1)/usr/lib/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/gstreamer-$(GST_VERSION)/libgst*.{la,so} \
		$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/pkgconfig/gstreamer*-$(GST_VERSION).pc \
		$(1)/usr/lib/pkgconfig/ \
	)
	$(INSTALL_DIR) $(2)/share/aclocal
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/share/aclocal/* \
		$(2)/share/aclocal/ \
	)
endef

define Package/gstreamer/install
	/bin/true
endef

define Package/gstreamer-utils/install
	$(INSTALL_DIR) $(1)/usr/bin
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/bin/gst-feedback* \
		./usr/bin/gst-launch* \
		./usr/bin/gst-inspect* \
		./usr/bin/gst-typefind* \
		./usr/bin/gst-xmlinspect* \
		./usr/bin/gst-xmllaunch* \
		$(1)/usr/bin/ \
	)
endef

define Package/libgstreamer/install
	$(INSTALL_DIR) $(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/libgstbase-$(GST_VERSION).so.* \
		./usr/lib/libgstreamer-$(GST_VERSION).so.* \
		$(1)/usr/lib/ \
	)
	$(INSTALL_DIR) $(1)/usr/lib/gstreamer-$(GST_VERSION)
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/gstreamer-$(GST_VERSION)/libgst*.so \
		$(1)/usr/lib/gstreamer-$(GST_VERSION)/ \
	)
endef


# 1: short name
# 2: description
# 3: dependencies on other gstreamer libraries (short name)
# 4: dependencies on other packages
define GstBuildLibrary

  define Package/libgst$(1)
    $(call Package/gstreamer/Default)
    TITLE+= $(2) library (core)
    DEPENDS+= +libgstreamer $$(foreach p,$(3),+libgst$$(p)) $(4)
  endef

  define Package/libgst$(1)/description
   $(call Package/gstreamer/description/Default)
   .
   This package contains the GStreamer $(2) library.
  endef

  define Package/libgst$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib
	( cd $(PKG_INSTALL_DIR); $(CP) \
		./usr/lib/libgst$(1)-$(GST_VERSION).so.* \
		$$(1)/usr/lib/ \
	)
  endef

  $$(eval $$(call BuildPackage,libgst$(1)))
endef

$(eval $(call GstBuildLibrary,check,check unit testing))
$(eval $(call GstBuildLibrary,controller,dynamic parameter control))
$(eval $(call GstBuildLibrary,dataprotocol,data protocol))
$(eval $(call GstBuildLibrary,net,network classes))

$(eval $(call BuildPackage,gstreamer))
$(eval $(call BuildPackage,gstreamer-utils))
$(eval $(call BuildPackage,libgstreamer))

