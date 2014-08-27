DESTDIR		?=
PREFIX		?= /usr
CFLAGS		?= -O2 -Wall
LDFLAGS		?=
LDLIBS		?=

STRIP		?= strip
INSTALL		?= install

CALVARIA_BIN	:= calvaria
CAL2P54_BIN	:= cal2p54

all: $(CALVARIA_BIN) $(CAL2P54_BIN)

$(CALVARIA_BIN): calvaria.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@

$(CAL2P54_BIN): cal2p54.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
	$(STRIP) $@

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

install: $(CALVARIA_BIN) $(CAL2P54_BIN)
	$(INSTALL) -d $(DESTDIR)/$(PREFIX)/bin
	$(INSTALL) -m 0755 $(CALVARIA_BIN) $(DESTDIR)/$(PREFIX)/bin/
	$(INSTALL) -m 0755 $(CAL2P54_BIN) $(DESTDIR)/$(PREFIX)/bin/

clean:
	rm -f $(CALVARIA_BIN) $(CAL2P54_BIN) *.o
