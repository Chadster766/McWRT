Source-Makefile: feeds/packages/libs/rrdtool-1.0.x/Makefile
Package: librrd1
Version: 1.0.50-2
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +zlib
Menu-Depends: 
Provides: 
Section: libs
Category: Libraries
Title: Round Robin Database (RRD) management library
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: rrdtool-1.0.50.tar.gz
Type: ipkg
Description:  RRD is the Acronym for Round Robin Database. RRD is a system to store and
 display time-series data (i.e. network bandwidth, machine-room temperature,
 server load average). It stores the data in a very compact way that will
 not expand over time, and it presents useful graphs by processing the data
 to enforce a certain data density. It can be used either via simple wrapper
 scripts (from shell or Perl) or via frontends that poll network devices and
 put friendly user interface on it.

 This is version 1.0.x with cgilib-0.4, gd1.3 and libpng-1.0.9 linked into
 librrd.so. The library is much smaller compared to the 1.2.x version with
 separate dynamic linked libraries.
 This package contains a shared library, used by other programs.
http://oss.oetiker.ch/rrdtool/
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: rrdcgi1
Version: 1.0.50-2
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +librrd1
Menu-Depends: 
Provides: 
Section: utils
Category: Utilities
Title: Round Robin Database (RRD) CGI graphing tool
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: rrdtool-1.0.50.tar.gz
Type: ipkg
Description:  RRD is the Acronym for Round Robin Database. RRD is a system to store and
 display time-series data (i.e. network bandwidth, machine-room temperature,
 server load average). It stores the data in a very compact way that will
 not expand over time, and it presents useful graphs by processing the data
 to enforce a certain data density. It can be used either via simple wrapper
 scripts (from shell or Perl) or via frontends that poll network devices and
 put friendly user interface on it.

 This is version 1.0.x with cgilib-0.4, gd1.3 and libpng-1.0.9 linked into
 librrd.so. The library is much smaller compared to the 1.2.x version with
 separate dynamic linked libraries.
 This package contains the rrdcgi tool used to create web pages containing
 RRD graphs based on templates.
http://oss.oetiker.ch/rrdtool/
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: rrdtool1
Version: 1.0.50-2
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +librrd1
Menu-Depends: 
Provides: 
Section: utils
Category: Utilities
Title: Round Robin Database (RRD) management tools
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: rrdtool-1.0.50.tar.gz
Type: ipkg
Description:  RRD is the Acronym for Round Robin Database. RRD is a system to store and
 display time-series data (i.e. network bandwidth, machine-room temperature,
 server load average). It stores the data in a very compact way that will
 not expand over time, and it presents useful graphs by processing the data
 to enforce a certain data density. It can be used either via simple wrapper
 scripts (from shell or Perl) or via frontends that poll network devices and
 put friendly user interface on it.

 This is version 1.0.x with cgilib-0.4, gd1.3 and libpng-1.0.9 linked into
 librrd.so. The library is much smaller compared to the 1.2.x version with
 separate dynamic linked libraries.
 This package contains command line tools used to manage RRDs.
http://oss.oetiker.ch/rrdtool/
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@


