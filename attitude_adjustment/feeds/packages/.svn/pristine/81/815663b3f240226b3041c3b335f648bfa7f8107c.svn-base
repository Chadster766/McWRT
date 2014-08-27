/* HUAWEI 3G modems activator
   Copyright (C) 2006 bobovsky bobovsky@kanoistika.sk  GPL
   Copyright (C) 2009 Nuno Goncalves nunojpg@gmail.com GPL
   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License2.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <usb.h>

struct usb_dev_handle *devh;

void release_usb_device(int dummy) {
	int ret;
	ret = usb_release_interface(devh, 0);

	if (!ret)
		printf("failed to release interface: %d\n", ret);

	usb_close(devh);

	if (!ret)
		printf("failed to close interface: %d\n", ret);

	exit(1);
}

void list_devices() {
	struct usb_bus *bus;
	for (bus = usb_get_busses(); bus; bus = bus->next) {
	struct usb_device *dev;

	for (dev = bus->devices; dev; dev = dev->next)
		printf("0x%04x 0x%04x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
    }
}    

struct usb_device *find_device() {
	struct usb_bus *bus;

	for (bus = usb_get_busses(); bus; bus = bus->next) {
		struct usb_device *dev;

		for (dev = bus->devices; dev; dev = dev->next) {
		if (dev->descriptor.idVendor == 0x12d1 && dev->descriptor.idProduct == 0x1001 || //Huawei E169, Huawei E169G
		    dev->descriptor.idVendor == 0x12d1 && dev->descriptor.idProduct == 0x1003)   //Huawei E220, Huawei E156G
		return dev;
		}
	}
	return NULL;
}

int main(int argc, char **argv) {
	int ret, vendor, product;
	struct usb_device *dev;
	char buf[65535], *endptr;

	usb_init();
	usb_find_busses();
	usb_find_devices();

	printf("Searching modem...");
	dev = find_device();

	if(dev == NULL){
		printf("No supported modem found\n");
		return 1;
	}

	printf("found supported modem!\n");

	assert(dev);
	devh = usb_open(dev);
	assert(devh);

	signal(SIGTERM, release_usb_device);

	ret = usb_get_descriptor(devh, 0x0000001, 0x0000000, buf, 0x0000012);
	usleep(1*1000);
	ret = usb_get_descriptor(devh, 0x0000002, 0x0000000, buf, 0x0000009);
	usleep(1*1000);
	ret = usb_get_descriptor(devh, 0x0000002, 0x0000000, buf, 0x0000020);
	usleep(1*1000);
	ret = usb_control_msg(devh, USB_TYPE_STANDARD + USB_RECIP_DEVICE, USB_REQ_SET_FEATURE, 00000001, 0, buf, 0, 1000);

	ret = usb_close(devh);
	assert(ret == 0);
	
	printf("Modem poked!\n");
	return 0;
}
