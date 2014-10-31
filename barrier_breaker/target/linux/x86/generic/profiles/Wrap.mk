#
# Copyright (C) 2006,2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Wrap
  NAME:=PCEngines WRAP
  PACKAGES:=kmod-i2c-scx200 kmod-natsemi kmod-leds-wrap kmod-gpio-scx200 kmod-wdt-scx200 kmod-hwmon-pc87360
endef

define Profile/Wrap/Description
	Package set compatible with the PCEngines WRAP. Contains I2C/LEDS/GPIO/Sensors support
endef
$(eval $(call Profile,Wrap))
