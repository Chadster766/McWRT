#
# Copyright (C) 2007-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wt
PKG_VERSION:=3.1.9
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.sourceforge.net/project/witty/wt/$(PKG_VERSION)
PKG_MD5SUM:=ee9490d5f24a47766e54f630ba0da2bb

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

PKG_INSTALL:=1

define Package/wt/Default
  SECTION:=libs
  CATEGORY:=Libraries
  URL:=http://www.webtoolkit.eu/wt
endef

define Package/wt
  $(call Package/wt/Default)
  MENU:=1
  DEPENDS:= +libstdcpp +boost-date_time +boost-system +boost-filesystem +boost-thread +boost-program_options +boost-signals +boost-regex +libpthread
  TITLE:=C++ Web Toolkit
endef

define Package/wt-resources
  $(call Package/wt/Default)
  DEPENDS:=wt
  TITLE:=Optional Resource Files
endef

define Package/wt-fcgi
  $(call Package/wt/Default)
  DEPENDS:=wt +fcgixx
  TITLE:=FastCGI Connector
endef

define Package/wt-http
  $(call Package/wt/Default)
  DEPENDS:=wt +PACKAGE_wt_http_zlib:zlib +PACKAGE_wt_http_ssl:libopenssl
  TITLE:=Built in HTTP Server
endef

define Package/wt-http/config
config PACKAGE_wt_http_zlib
    bool "Enable zlib compression option"
    depends on PACKAGE_wt-http
    default n
config PACKAGE_wt_http_ssl
    bool "Enable ssl option"
    depends on PACKAGE_wt-http
    default n
endef

define Package/wt-dbo
  $(call Package/wt/Default)
  DEPENDS:=wt
  TITLE:=Object Relational Mapping
endef

define Package/wt-dbosqlite3
  $(call Package/wt/Default)
  DEPENDS:=wt +wt-dbo
  TITLE:=Sqlite3 Dbo Backend
endef

define Package/wt-resources/conffiles
/usr/share/Wt
endef

# TODO: some parts of the code show that -DHAVE_GNU_REGEX can be defined to avoid dependence or boost-regex
# But that's turning out not to be the case (some portions still depend on boost-regex)
TARGET_CFLAGS += $(FPIC)
CMAKE_OPTIONS += \
	-DCMAKE_BUILD_TYPE=MinSizeRel \
	-DCMAKE_CXX_FLAGS_MINSIZEREL:STRING="-DNDEBUG -DWT_NO_SPIRIT -DWT_NO_XSS_FILTER" \
	-DSHARED_LIBS:BOOL=ON \
	-DBUILD_TESTS=OFF \
	-DBUILD_EXAMPLES=OFF \
	-DINSTALL_RESOURCES=ON \
	-DENABLE_GM=OFF \
	-DENABLE_HARU=OFF \
	-DENABLE_PANGO=OFF \
	-DENABLE_EXT=OFF \
	-DENABLE_POSTGRES=OFF \
	-DENABLE_QT4=OFF \
	-DWT_NO_STD_LOCALE=ON \
	-DWT_NO_STD_WSTRING=ON

ifndef CONFIG_PACKAGE_wt-http
CMAKE_OPTIONS += \
	-DCONNECTOR_HTTP=OFF
endif

ifndef CONFIG_PACKAGE_wt_http_zlib
CMAKE_OPTIONS += \
	-DHTTP_WITH_ZLIB=OFF
endif

ifndef CONFIG_PACKAGE_wt_http_ssl
CMAKE_OPTIONS += \
	-DHTTP_WITH_SSL=OFF
endif

ifndef CONFIG_PACKAGE_wt-fcgi
CMAKE_OPTIONS += \
	-DCONNECTOR_FCGI=OFF
endif

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) -a $(PKG_INSTALL_DIR)/usr/include/Wt $(1)/usr/include

	$(INSTALL_DIR) $(1)/usr/share/cmake-2.4/Modules
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cmake-2.4/Modules/FindWt.cmake $(1)/usr/share/cmake-2.4/Modules

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwt*.so* $(1)/usr/lib
endef

define BuildPlugin
  define Package/$(1)/install
	$(INSTALL_DIR) $$(1)/usr/lib
	for m in $(2); do \
		$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwt$$$$$$$${m}.so* $$(1)/usr/lib/ ; \
	done
  endef

  $$(eval $$(call BuildPackage,$(1)))
endef

define Package/wt/install
	$(INSTALL_DIR) $(1)/etc/wt
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/wt_config.xml $(1)/etc/wt

	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libwt.so* $(1)/usr/lib
endef

define Package/wt/conffiles
/etc/wt
endef

define Package/wt-resources/install
	$(INSTALL_DIR) $(1)/usr/share
	$(CP) -a $(PKG_INSTALL_DIR)/usr/share/Wt $(1)/usr/share
endef

$(eval $(call BuildPackage,wt))
$(eval $(call BuildPackage,wt-resources))
$(eval $(call BuildPlugin,wt-http,http))
$(eval $(call BuildPlugin,wt-fcgi,fcgi))
$(eval $(call BuildPlugin,wt-dbo,dbo))
$(eval $(call BuildPlugin,wt-dbosqlite3,dbosqlite3))
