define Profile/Generic
  NAME:=Generic - all boards
  PACKAGES:= \
	kmod-dm9000 \
	kmod-i2c-core kmod-i2c-falcon-lantiq kmod-eeprom-at24 \
	kmod-spi-bitbang kmod-spi-gpio kmod-eeprom-at25 \
	gpon-dti-agent
endef

$(eval $(call Profile,Generic))


