BOARDNAME:=Traverse Geos
FEATURES:=squashfs jffs2 ext4 pci usb gpio
DEFAULT_PACKAGES += \
            kmod-crypto-hw-geode kmod-crypto-ocf \
            kmod-gpio-cs5535-new kmod-gpio-nsc \
            kmod-wdt-geode kmod-cs5535-clockevt kmod-cs5535-mfgpt \
            kmod-cs5536 \
            kmod-hwmon-core kmod-hwmon-lm90 \
            kmod-8139cp kmod-solos-pci kmod-sched \
            kmod-i2c-core kmod-i2c-gpio \
            kmod-i2c-algo-bit kmod-i2c-algo-pca kmod-i2c-algo-pcf \
            kmod-i2c-scx200-acb \
            kmod-usb-core kmod-usb2 kmod-usb-ohci \
            kmod-cfg80211 kmod-mac80211 \
            kmod-mppe kmod-pppoa kmod-pppol2tp \
            kmod-ath5k kmod-ath9k \
            kmod-leds-gpio kmod-input-gpio-keys-polled \
            kmod-button-hotplug \
            kmod-ledtrig-heartbeat kmod-ledtrig-gpio \
            kmod-ledtrig-netdev \
            kmod-cpu-msr \
            soloscli linux-atm br2684ctl ppp-mod-pppoa pppdump pppstats \
            hwclock wpad flashrom tc

CS5535_MASK:=0x0E000048

CFLAGS += -march=geode -Os -mmmx -m3dnow -fno-align-jumps -fno-align-functions \
        -fno-align-labels -fno-align-loops -pipe -fomit-frame-pointer

define Target/Description
    Build firmware images for Traverse Geos board
endef

define KernelPackage/gpio-cs5535-new/install
     sed -i -r -e 's/$$$$$$$$/ mask=$(CS5535_MASK)/' $(1)/etc/modules.d/??-gpio-cs5535-new
endef
