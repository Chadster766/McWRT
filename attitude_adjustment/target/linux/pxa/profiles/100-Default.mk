#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default Profile
  PACKAGES:=
endef

define Profile/Default/Description
	Default PXA Profile
endef
$(eval $(call Profile,Default))

