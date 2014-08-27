#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ditg
PKG_VERSION:=2.8.0-rc1
PKG_RELEASE:=1

PKG_SOURCE:=D-ITG-$(PKG_VERSION).tgz
PKG_SOURCE_URL:=http://www.grid.unina.it/software/ITG/codice
PKG_MD5SUM:=609736113de729df833ac691d1c44ead

PKG_BUILD_DIR:=$(BUILD_DIR)/D-ITG-$(PKG_VERSION)

include $(INCLUDE_DIR)/uclibc++.mk
include $(INCLUDE_DIR)/package.mk

define Package/ditg
  TITLE:=D-ITG (Distributed Internet Traffic Generator)
  SECTION:=net
  CATEGORY:=Network
  URL:=http://www.grid.unina.it/software/ITG/
  MAINTAINER:=Giovanni Di Stasi <giovanni.distasi@unina.it>
  DEPENDS:=+libpthread $(CXX_DEPENDS)
endef

define Package/ditg/description
  D-ITG (Distributed Internet Traffic Generator) is a platform capable
  to produce traffic at packet level accurately replicating appropriate
  stochastic processes for both IDT (Inter Departure Time) and PS
  (Packet Size) random variables (exponential, uniform, cauchy, normal,
  pareto, ...). D-ITG supports both IPv4 and IPv6 traffic generation and
  it is capable to generate traffic at network, transport, and
  application layer. We believe that D-ITG shows interesting properties
  when compared to other traffic generators.
endef

MAKE_PATH := src

MAKE_FLAGS += \
	CXXOPT="$(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -DMULTIPORT -DARM"

define Package/ditg/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bin/ITG{Send,Recv,Log,Dec} $(1)/usr/bin/
endef

$(eval $(call BuildPackage,ditg))
