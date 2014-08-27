#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2OPTS += --little-endian --pagesize=0x800 --no-cleanmarkers --pad
JFFS2_BLOCKSIZE=128k

# override add_jffs2_mark
define add_jffs2_mark
	echo "do nothing"
endef

# override Image/mkfs/prepare/default
define Image/mkfs/prepare/default
	# Use symbolic permissions to avoid clobbering SUID/SGID/sticky bits
	- $(FIND) $(TARGET_DIR) -type f -not -perm +0100 -not -name 'ssh_host*' -not -name 'shadow' -print0 | $(XARGS) -0 chmod u+rw,g+r,o+r
	- $(FIND) $(TARGET_DIR) -type f -perm +0100 -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	- $(FIND) $(TARGET_DIR) -type d -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	$(INSTALL_DIR) $(TARGET_DIR)/tmp
	chmod 1777 $(TARGET_DIR)/tmp
	ln -sf /etc/preinit $(TARGET_DIR)/sbin/init
	ln -sf /bin/busybox $(TARGET_DIR)/sbin/busybox_init
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(KDIR)/uImage
	mkdir -p $(TARGET_DIR)/boot
	cp $(LINUX_DIR)/arch/arm/boot/uImage $(TARGET_DIR)/boot/uImage
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
# do mach-id fixup here, if needed
	cp $(KDIR)/uImage $(BIN_DIR)/$(IMG_PREFIX)-uImage
endef

define Image/Build
	$(if $(Image/Build/$(1)), \
		$(call Image/Build/$(1),$(1)), \
		$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).img \
	)
endef

define Image/Build/jffs2-128k
	( \
		dd if=$(KDIR)/uImage bs=128k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).img
	rm -f $(KDIR)/root.$(1)
endef

define Image/Build/squashfs
	$(STAGING_DIR_HOST)/bin/mkimage -A arm -O linux -T multi \
	-C none -a 0x00008000 -e 0x00008000 -n 'Linux-$(LINUX_VERSION)-initrd' \
	-d $(KDIR)/zImage:$(KDIR)/root.squashfs $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).uimg.tmp
	dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).uimg.tmp of=$(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).uimg bs=2048 conv=sync
	rm $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).uimg.tmp

	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	( \
		dd if=$(KDIR)/uImage bs=4096k conv=sync; \
		dd if=$(KDIR)/root.$(1) bs=128k conv=sync; \
	) > $(BIN_DIR)/$(IMG_PREFIX)-$(PROFILE)-$(1).img
endef

$(eval $(call BuildImage))
