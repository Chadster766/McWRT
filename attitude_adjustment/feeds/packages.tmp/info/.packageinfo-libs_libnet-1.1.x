Source-Makefile: feeds/packages/libs/libnet-1.1.x/Makefile
Package: libnet1
Version: 1.1.2.1-2
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +libpcap
Menu-Depends: 
Provides: 
Section: libs
Category: Libraries
Title: Low-level packet creation library (v1.1.x)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: libnet.tar.gz
Type: ipkg
Description: Low-level packet creation library (v1.1.x)
http://www.packetfactory.net/libnet/
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@


