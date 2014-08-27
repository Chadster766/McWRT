Source-Makefile: feeds/packages/libs/zaptel-1.4.x/Makefile
Package: kmod-zaptel14
Submenu: Voice over IP
Version: <LINUX_VERSION>+1.4.12.1-4
Depends: 
Menu-Depends: 
Provides: 
Section: kernel
Category: Kernel modules
Title: Zaptel (kernel module)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description:  This package contains the Zaptel ztdummy driver.  Broken on x86 due to lack
 of RTC on some hardware.
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: kmod-zaptel14-dummy
Submenu: Voice over IP
Version: <LINUX_VERSION>+1.4.12.1-4
Depends:  +kmod-zaptel14
Menu-Depends: 
Provides: 
Section: kernel
Category: Kernel modules
Title: Zaptel Dummy (timer; kernel module)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description: Zaptel Dummy (timer; kernel module)
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: kmod-zaptel14-wcusb
Submenu: Voice over IP
Version: <LINUX_VERSION>+1.4.12.1-4
Depends:  +kmod-zaptel14
Menu-Depends: 
Provides: 
Section: kernel
Category: Kernel modules
Title: Zaptel (kernel module)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description:  This package contains the Zaptel S100U usb driver.
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: kmod-zaptel14-wctdm
Submenu: Voice over IP
Version: <LINUX_VERSION>+1.4.12.1-4
Depends:  +kmod-zaptel14
Menu-Depends: 
Provides: 
Section: kernel
Category: Kernel modules
Title: Zaptel (kernel module)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description:  This package contains the Zaptel wctdm driver.
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: zaptel14-util
Version: 1.4.12.1-4
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +kmod-zaptel14 +zaptel14-libtonezone +libnewt
Menu-Depends: 
Provides: 
Section: utils
Category: Utilities
Title: Zaptel utils
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description:  This package contains the zaptel utils.
http://ftp.digium.com/pub/zaptel/releases
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@

Package: zaptel14-libtonezone
Version: 1.4.12.1-4
Depends: +libc +USE_EGLIBC:librt +USE_EGLIBC:libpthread +kmod-zaptel14
Menu-Depends: 
Provides: 
Section: libs
Category: Libraries
Title: Zaptel libtonezone (library)
Maintainer: OpenWrt Developers Team <openwrt-devel@openwrt.org>
Source: zaptel-1.4.12.1.tar.gz
Type: ipkg
Description:  This package contains the libraries for accessing zaptel/dummy drivers.
http://ftp.digium.com/pub/zaptel/releases
OpenWrt Developers Team <openwrt-devel@openwrt.org>
@@


