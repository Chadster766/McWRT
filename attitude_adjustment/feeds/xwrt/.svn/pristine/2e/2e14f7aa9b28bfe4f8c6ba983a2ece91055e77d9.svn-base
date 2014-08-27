CC = gcc
CFLAGS += -Wall
LDFLAGS +=
RM = rm -f

INT2HUMAN_SOURCES = int2human/main.c int2human/human_readable.c
WEPKEYGEN_SOURCES = wepkeygen/keygen.c wepkeygen/md5.c
BSTRIP_SOURCES = bstrip.c
WEBIF_PAGE_SOURCES = webif-page.c
WEBIFMETABIN_SOURCES = $(INT2HUMAN_SOURCES) $(WEPKEYGEN_SOURCES) \
	$(BSTRIP_SOURCES) $(WEBIF_PAGE_SOURCES) webifmetabin.c

all: webifmetabin

webifmetabin: $(WEBIFMETABIN_SOURCES)
	$(CC) $(CFLAGS) -D_METAPACK $(LDFLAGS) $(WEBIFMETABIN_SOURCES) -o $@

int2human/int2human: $(INT2HUMAN_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(INT2HUMAN_SOURCES) -o $@

wepkeygen/wepkeygen: $(WEPKEYGEN_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WEPKEYGEN_SOURCES) -o $@

bstrip: $(BSTRIP_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(BSTRIP_SOURCES) -o $@

webif-page: $(WEBIF_PAGE_SOURCES)
	$(CC) $(CFLAGS) $(LDFLAGS) $(WEBIF_PAGE_SOURCES) -o $@

clean:
	$(RM) *~ */*~ webifmetabin int2human/int2human wepkeygen/wepkeygen bstrip webif-page

.PHONY: clean
