#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=petitboot
PKG_REV:=93b2c2e0f0ca46d2a823b33cdfa44d082e9e8d10
PKG_VERSION:=20090709
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=git://ozlabs.org/home/jk/git/petitboot
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)

PKG_INSTALL=1

include $(INCLUDE_DIR)/package.mk

define Package/petitboot
	SECTION:=boot
	CATEGORY:=Boot Loaders
	TITLE:=A kexec based bootloader
	URL:=http://ozlabs.org/~jk/projects/petitboot/
	MAINTAINER:=Geoff Levand <geoffrey.levand@am.sony.com>
	DEPENDS:=@TARGET_ps3 +kexec-tools +libncurses +libtwin +udev +ps3-utils
endef

define Package/petitboot/description
	Petitboot is a Linux kexec based bootloader with both
	graphical and command-line user interfaces. The OpenWRT
	Petitboot package has configuration options that can be
	used to specify which Petitboot components to build, and
	can also be used to specify that Petitboot should be run
	automatically at system start-up.

	Petitboot supports loading Linux kernel and initrd images
	from any device that can be mounted by Linux.  It can
	handle gzipped images and can also load images from the
	network using TFTP, NFS, HTTP, HTTPS, and SFTP.  Petitboot
	looks for bootloader config files on mountable devices in
	the system, and also uses the boot configuration sent to
	it from a DHCP server.
endef

define Package/petitboot/config
	source "$(SOURCE)/petitboot-config.in"
endef

petitboot-config-args-y$(CONFIG_PETITBOOT_CUI) += --without-ncurses
petitboot-config-args-y$(CONFIG_PETITBOOT_GUI) += --without-twin
petitboot-config-args-y$(CONFIG_PETITBOOT_PS3) += --enable-ps3=no

petitboot-cppflags-$(CONFIG_PETITBOOT_DEBUG) += -DDEBUG
petitboot-cppflags-y$(CONFIG_PETITBOOT_DEBUG) += -DNDEBUG

petitboot-extra-bin-$(CONFIG_PETITBOOT_CUI) += /usr/sbin/pb-cui
petitboot-extra-bin-$(CONFIG_PETITBOOT_GUI) += /usr/sbin/pb-twin
petitboot-extra-data-$(CONFIG_PETITBOOT_GUI) += /usr/share/petitboot/artwork/*

# The platform inittab and/or initrun should check for /usr/sbin/petitboot
# and run it on startup if found.

petitboot-install-autorun-$(CONFIG_PETITBOOT_AUTORUN_CUI) = \
	ln -sf pb-cui $(1)/usr/sbin/petitboot
petitboot-install-autorun-$(CONFIG_PETITBOOT_AUTORUN_GUI) = \
	ln -sf pb-twin $(1)/usr/sbin/petitboot

CONFIGURE_ARGS += $(petitboot-config-args-y) \
	PACKAGE_VERSION=$(shell echo ${HOSTNAME})-$(shell date +%y.%m.%d-%H.%M)

EXTRA_CPPFLAGS += $(petitboot-cppflags-y)

define Build/Prepare
	$(call Build/Prepare/Default)
	(cd $(PKG_BUILD_DIR) && $(BASH) -x ./bootstrap)
endef

define Package/petitboot/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) \
		$(addprefix $(PKG_INSTALL_DIR),$(petitboot-extra-bin-y)) \
		$(PKG_INSTALL_DIR)/usr/sbin/pb-discover \
		$(PKG_INSTALL_DIR)/usr/sbin/pb-event \
		$(1)/usr/sbin/

	$(INSTALL_DIR) $(1)/usr/share/petitboot/
	$(INSTALL_DATA) \
		$(addprefix $(PKG_INSTALL_DIR),$(petitboot-extra-data-y)) \
		$(1)/usr/share/petitboot/

	$(INSTALL_DIR) $(1)/etc/udev/rules.d
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/utils/99-petitboot.rules \
		$(1)/etc/udev/rules.d/

	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_DATA) -T $(PKG_BUILD_DIR)/utils/udhcpc $(1)/etc/udhcpc.user

	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) \
		./files/petitboot.init \
		$(1)/etc/init.d/petitboot

	$(petitboot-install-autorun-y)
endef

$(eval $(call BuildPackage,petitboot))
