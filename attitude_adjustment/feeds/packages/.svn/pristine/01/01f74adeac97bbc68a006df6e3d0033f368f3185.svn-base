# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=netpipe
PKG_VERSION:=3.7.1
PKG_RELEASE:=1

PKG_SOURCE:=NetPIPE-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.scl.ameslab.gov/netpipe/code
PKG_MD5SUM:=cc458a9154f6b8c6da9fae16627beec4

PKG_BUILD_DIR:=$(BUILD_DIR)/NetPIPE-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/netpipe
  SECTION:=net
  CATEGORY:=Network
  TITLE:=A Network Protocol Independent Performance Evaluator
  URL:=http://www.scl.ameslab.gov/netpipe/
endef

define Package/netpipe/description
	NetPIPE is a protocol independent performance tool that visually
	represents the network performance under a variety of conditions. It
	performs simple ping-pong tests, bouncing messages of increasing size
	between two processes, whether across a network or within an SMP
	system. Message sizes are chosen at regular intervals, and with slight
	perturbations, to provide a complete test of the communication system.
	Each data point involves many ping-pong tests to provide an accurate
	timing. Latencies are calculated by dividing the round trip time in
	half for small messages ( < 64 Bytes ).
endef

define Build/Configure
endef

define Package/netpipe/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/NPtcp $(1)/usr/bin/
endef

$(eval $(call BuildPackage,netpipe))
