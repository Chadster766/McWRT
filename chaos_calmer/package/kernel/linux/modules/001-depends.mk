#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define AddDepends/crc16
  DEPENDS+= +kmod-lib-crc16 $(1)
endef

define AddDepends/hid
  DEPENDS+= +kmod-hid $(1)
endef

define AddDepends/input
  DEPENDS+= +kmod-input-core $(1)
endef


define AddDepends/nls
  DEPENDS+= +kmod-nls-base $(foreach cp,$(1),+kmod-nls-$(cp))
endef

define AddDepends/rfkill
  DEPENDS+= +USE_RFKILL:kmod-rfkill $(1)
endef


define AddDepends/rtc
  DEPENDS+= @RTC_SUPPORT $(1)
endef
