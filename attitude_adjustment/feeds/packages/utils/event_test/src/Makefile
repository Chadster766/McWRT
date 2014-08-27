#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under GNU General Public Licese v2.
# See /LICENSE for more information.
#

all: event_test

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $^

event_test: evtest.o
	$(CC) -o $@ $^

clean:
	rm -f *.o event_test
