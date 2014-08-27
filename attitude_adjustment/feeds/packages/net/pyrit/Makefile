#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=pyrit
PKG_VERSION:=0.4.0
PKG_RELEASE:=1

PKG_SOURCE_URL:=http://pyrit.googlecode.com/files
PKG_SOURCE:=pyrit-0.4.0.tar.gz
PKG_MD5SUM:=7258b6f3dacfb09736ddeed2a379df2d

include $(INCLUDE_DIR)/package.mk
$(call include_mk, python-package.mk)

define Package/pyrit
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+python +libopenssl +libpcap
  TITLE:=A GPGPU-driven WPA/WPA2-PSK key cracker
  URL:=http://code.google.com/p/pyrit/
endef

define Package/pyrit/description
 A GPGPU-driven WPA/WPA2-PSK key cracker
 Pyrit exploits the computational power of many-core- and GPGPU-
 -platforms to create massive databases, pre-computing part of the 
 WPA/WPA2-PSK authentication phase in a space-time tradeoff. 
 It is a powerful attack against one of the world's most used security
 protocols
endef

define Build/Compile
	$(call Build/Compile/PyMod,., \
		install --prefix="/usr" --root="$(PKG_INSTALL_DIR)" \
	)
endef

define Package/pyrit/install
	$(INSTALL_DIR) $(1)
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,pyrit))
