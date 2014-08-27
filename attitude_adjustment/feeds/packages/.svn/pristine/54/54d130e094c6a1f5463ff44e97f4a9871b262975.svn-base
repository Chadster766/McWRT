/*
 * Calvaria - Maemo CAL partition variable access tool.
 *
 * Copyright (c) 2011 Michael Buesch <mb@bu3sch.de>
 *
 * Licensed under the GNU General Public License version 2.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>


#define _packed		__attribute__((__packed__))

typedef uint16_t le16_t;
typedef uint32_t le32_t;

struct header {
	char magic[4];		/* Magic sequence */
	uint8_t type;		/* Type number */
	uint8_t index;		/* Index number */
	le16_t flags;		/* Flags */
	char name[16];		/* Human readable section name */
	le32_t length;		/* Payload length */
	le32_t datasum;		/* Data CRC32 checksum */
	le32_t hdrsum;		/* Header CRC32 checksum */
} _packed;

#define HDR_MAGIC	"ConF"

#define INDEX_LAST	(0xFF + 1)


static char toAscii(char c)
{
	if (c >= 32 && c <= 126)
		return c;
	return '.';
}

static void dump_bytes(FILE *outstream, const void *_buf, size_t size)
{
	const char *buf = _buf;
	size_t i, ascii_cnt = 0;
	char ascii[17] = { 0, };

	for (i = 0; i < size; i++) {
		if (i % 16 == 0) {
			if (i != 0) {
				fprintf(outstream, "  |%s|\n", ascii);
				ascii[0] = 0;
				ascii_cnt = 0;
			}
			fprintf(outstream, "[%04X]: ", (unsigned int)i);
		}
		fprintf(outstream, " %02X", buf[i] & 0xFF);
		ascii[ascii_cnt] = toAscii(buf[i]);
		ascii[ascii_cnt + 1] = 0;
		ascii_cnt++;
	}
	if (ascii[0]) {
		if (size % 16) {
			for (i = 0; i < 16 - (size % 16); i++)
				fprintf(outstream, "   ");
		}
		fprintf(outstream, "  |%s|\n", ascii);
	}
}

static uint32_t crc32(uint32_t crc, const void *_data, size_t size)
{
	const uint8_t *data = _data;
	uint8_t value;
	unsigned int bit;
	size_t i;
	const uint32_t poly = 0xEDB88320;

	for (i = 0; i < size; i++) {
		value = data[i];
		for (bit = 8; bit; bit--) {
			if ((crc & 1) != (value & 1))
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
			value >>= 1;
		}
	}

	return crc;
}

static inline uint16_t le16_to_cpu(le16_t x)
{
	uint8_t *bytes = (uint8_t *)&x;
	uint16_t ret;

	ret = bytes[0];
	ret |= (uint16_t)(bytes[1]) << 8;

	return ret;
}

static inline uint32_t le32_to_cpu(le32_t x)
{
	uint8_t *bytes = (uint8_t *)&x;
	uint32_t ret;

	ret = bytes[0];
	ret |= (uint32_t)(bytes[1]) << 8;
	ret |= (uint32_t)(bytes[2]) << 16;
	ret |= (uint32_t)(bytes[3]) << 24;

	return ret;
}

static int is_header(void *data, size_t size)
{
	struct header *hdr = data;

	if (size < sizeof(struct header))
		return 0;
	if (memcmp(hdr->magic, HDR_MAGIC, sizeof(hdr->magic)) != 0)
		return 0;
	return 1;
}

void dump_bme_payload(FILE *outstream, const void *payload, size_t len)
{
	const uint8_t *pmm_area = payload;
	unsigned int group, element, pmm_offset;
	uint8_t active_group_mask;
	static const uint8_t zero_element[16] = { 0, };

	if (len != 0x600 || memcmp(payload, "BME-PMM-BLOCK01", 15) != 0) {
		fprintf(outstream, "ERROR: The section payload is not "
			"a BME PMM block\n");
		return;
	}

	active_group_mask = pmm_area[16];

	for (group = 0; group < 3; group++) {
		for (element = 0; element < 32; element++) {
			pmm_offset = group * 512 + element * 16;
			if (memcmp(pmm_area + pmm_offset, zero_element, 16) == 0)
				continue;
			fprintf(outstream, "-- BME-PMM area -- group %u%s, element %u:\n",
				group,
				(active_group_mask & (1 << group)) ? "" : " (INACTIVE)",
				element);
			dump_bytes(outstream, pmm_area + pmm_offset, 16);
		}
	}
}

typedef void (*payload_dumper_t)(FILE *, const void *payload, size_t len);

static payload_dumper_t get_payload_dumper(const char *section_name, int raw)
{
	if (!raw) {
		if (strcmp(section_name, "bme") == 0)
			return dump_bme_payload;
	}

	return dump_bytes;
}

static int dump_section(const struct header *hdr,
			const void *payload, size_t payload_len,
			int dump_payload,
			int dump_raw_payload,
			FILE *outstream)
{
	char name[sizeof(hdr->name) + 1] = { 0, };
	int hdrsum_ok, datasum_ok;
	payload_dumper_t dumper;

	memcpy(name, hdr->name, sizeof(hdr->name));
	hdrsum_ok = (crc32(0, hdr, sizeof(*hdr) - 4) == le32_to_cpu(hdr->hdrsum));
	datasum_ok = (crc32(0, payload, payload_len) == le32_to_cpu(hdr->datasum));

	fprintf(outstream, "Section:       %s\n", name);
	fprintf(outstream, "Type:          %u (0x%X)\n", hdr->type, hdr->type);
	fprintf(outstream, "Index:         %u (0x%X)\n", hdr->index, hdr->index);
	fprintf(outstream, "Flags:         0x%04X\n", le16_to_cpu(hdr->flags));
	fprintf(outstream, "Length:        %u (0x%X)\n",
		le32_to_cpu(hdr->length), le32_to_cpu(hdr->length));
	fprintf(outstream, "Data CRC32:    0x%08X (%s)\n", le32_to_cpu(hdr->datasum),
		datasum_ok ? "Ok" : "MISMATCH");
	fprintf(outstream, "Header CRC32:  0x%08X (%s)\n", le32_to_cpu(hdr->hdrsum),
		hdrsum_ok ? "Ok" : "MISMATCH");
	if (dump_payload) {
		dumper = get_payload_dumper(name, dump_raw_payload);
		dumper(outstream, payload, payload_len);
	}
	fprintf(outstream, "\n");

	return 0;
}

static void * map_file(const char *filepath, int readonly,
		       uint64_t *filelen)
{
	int fd;
	off_t len;
	void *data;

	fd = open(filepath, readonly ? O_RDONLY : O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open file %s: %s\n",
			filepath, strerror(errno));
		return NULL;
	}
	len = lseek(fd, 0, SEEK_END);
	if (len < 0 || lseek(fd, 0, SEEK_SET)) {
		fprintf(stderr, "Failed to calculate file length of %s: %s\n",
			filepath, strerror(errno));
		close(fd);
		return NULL;
	}

	data = mmap(NULL, len,
		    readonly ? PROT_READ : (PROT_READ | PROT_WRITE),
		    readonly ? MAP_PRIVATE : 0,
		    fd, 0);
	close(fd);
	if (data == MAP_FAILED) {
		fprintf(stderr, "Failed to MMAP file %s: %s\n",
			filepath, strerror(errno));
		return NULL;
	}
	madvise(data, len, MADV_SEQUENTIAL);

	*filelen = len;

	return data;
}

static void unmap_file(void *mapping, uint64_t len)
{
	if (mapping)
		munmap(mapping, len);
}

static int64_t find_section(void *start, uint64_t count,
			    int want_index, const char *want_name)
{
	int64_t offset = 0, found_offset = -1;
	uint8_t *data = start;
	struct header *hdr;
	char sectname[sizeof(hdr->name) + 1] = { 0, };
	uint32_t payload_len;
	int previous_index = -1;

	while (1) {
		/* Find header start */
		if (count < sizeof(struct header))
			break;
		if (!is_header(data + offset, count)) {
			count--;
			offset++;
			continue;
		}
		hdr = (struct header *)(data + offset);
		payload_len = le32_to_cpu(hdr->length);
		if (count - sizeof(struct header) < payload_len) {
			fprintf(stderr, "Premature EOF\n");
			return -1;
		}
		memcpy(sectname, hdr->name, sizeof(hdr->name));

		if (want_index == INDEX_LAST) {
			if ((int)hdr->index <= previous_index)
				goto next;
		} else {
			if (want_index >= 0 && want_index != hdr->index)
				goto next;
		}
		if (want_name && strcmp(sectname, want_name) != 0)
			goto next;

		/* Found it */
		found_offset = offset;
		if (want_index == INDEX_LAST)
			previous_index = hdr->index;
		else
			break;

next:
		count -= sizeof(struct header) + payload_len;
		offset += sizeof(struct header) + payload_len;
	}

	return found_offset;
}

static int dump_image(const char *filepath,
		      int want_section_index, const char *want_section_name,
		      int want_headers_only,
		      int dump_raw_payload,
		      FILE *outstream)
{
	int err, ret = 0;
	uint64_t filelen;
	uint64_t count, offset;
	int64_t find_offset;
	uint8_t *data, *section;
	struct header *hdr;
	uint32_t payload_len;

	data = map_file(filepath, 1, &filelen);
	if (!data)
		return -EIO;

	count = filelen;
	offset = 0;
	while (1) {
		find_offset = find_section(data + offset, count,
					   want_section_index, want_section_name);
		if (find_offset < 0)
			break;
		offset += find_offset;
		count -= find_offset;

		section = data + offset;
		hdr = (struct header *)section;
		payload_len = le32_to_cpu(hdr->length);

		err = dump_section(hdr, section + sizeof(struct header),
				   payload_len,
				   !want_headers_only,
				   dump_raw_payload,
				   outstream);
		if (err) {
			ret = err;
			goto out;
		}

		count -= sizeof(struct header) + payload_len;
		offset += sizeof(struct header) + payload_len;
	}
out:
	unmap_file(data, filelen);

	return ret;
}

static int write_payload(const char *filepath,
			 int want_section_index, const char *want_section_name,
			 FILE *outstream)
{
	int64_t find_offset;
	uint64_t filelen;
	uint8_t *data;
	struct header *hdr;

	data = map_file(filepath, 1, &filelen);
	if (!data)
		return -EIO;

	find_offset = find_section(data, filelen,
				   want_section_index, want_section_name);
	if (find_offset < 0) {
		fprintf(stderr, "Section %s, index %d not found\n",
			want_section_name, want_section_index);
		unmap_file(data, filelen);
		return -ESRCH;
	}

	hdr = (struct header *)(data + find_offset);
	if (fwrite(data + find_offset + sizeof(struct header),
		   le32_to_cpu(hdr->length), 1, outstream) != 1) {
		fprintf(stderr, "Could not write output data\n");
		unmap_file(data, filelen);
		return -EIO;
	}

	unmap_file(data, filelen);

	return 0;
}

static void usage(FILE *fd)
{
	fprintf(fd, "Calvaria - Maemo CAL partition variable access tool\n");
	fprintf(fd, "\n");
	fprintf(fd, "Usage: calvaria [OPTIONS] FILE\n");
	fprintf(fd, "\n");
	fprintf(fd, "Actions:\n");
	fprintf(fd, "  -d|--dump            Dump the contents of the image\n");
	fprintf(fd, "  -H|--headers         Dump the headers of the image, only\n");
	fprintf(fd, "  -p|--payload         Write the binary payload to stdout.\n");
	fprintf(fd, "                       Requires -i and -n to be set, too\n");
	fprintf(fd, "\n");
	fprintf(fd, "Options:\n");
	fprintf(fd, "  -i|--index NUMBER    Use this section index number\n");
	fprintf(fd, "  -n|--name STRING     Use this section name\n");
	fprintf(fd, "  -r|--raw             Dump raw data (for -d|--dump only)\n");
	fprintf(fd, "\n");
	fprintf(fd, "  -h|--help            Print this help text\n");
}

enum action {
	ACTION_NONE,
	ACTION_DUMP,
	ACTION_DUMPHDRS,
	ACTION_GETPAYLOAD,
};

int main(int argc, char **argv)
{
	int err, c, idx = 0;
	const char *filepath;
	enum action action = ACTION_NONE;
	int opt_index = -1;
	const char *opt_name = NULL;
	int opt_raw_payload = 0;

	static struct option long_options[] = {
		{ "dump", no_argument, 0, 'd', },
		{ "headers", no_argument, 0, 'H', },
		{ "payload", no_argument, 0, 'p', },
		{ "index", required_argument, 0, 'i', },
		{ "name", required_argument, 0, 'n', },
		{ "raw", no_argument, 0, 'r', },
		{ "help", no_argument, 0, 'h', },
		{ },
	};

	while (1) {
		c = getopt_long(argc, argv, "dHphi:n:r",
				long_options, &idx);
		if (c == -1)
			break;
		switch (c) {
		case 'h':
			usage(stdout);
			return 0;
		case 'd':
			action = ACTION_DUMP;
			break;
		case 'H':
			action = ACTION_DUMPHDRS;
			break;
		case 'p':
			action = ACTION_GETPAYLOAD;
			break;
		case 'i':
			if (strcasecmp(optarg, "last") == 0) {
				opt_index = INDEX_LAST;
			} else {
				if (sscanf(optarg, "%d", &opt_index) != 1 || opt_index < 0) {
					fprintf(stderr, "-i|--index is not a positive integer\n");
					return 1;
				}
			}
			break;
		case 'n':
			opt_name = optarg;
			break;
		case 'r':
			opt_raw_payload = 1;
			break;
		default:
			return 1;
		}
	}
	argc -= optind;
	argv += optind;
	if (action == ACTION_NONE) {
		fprintf(stderr, "No action specified.\n");
		return 1;
	}
	if (action == ACTION_GETPAYLOAD) {
		if (opt_index < 0 || !opt_name) {
			fprintf(stderr, "Required options -i|--index or -n|--name "
				"not specified for action -p|--payload\n");
			return 1;
		}
	}
	if (argc != 1) {
		usage(stderr);
		return 1;
	}
	filepath = argv[0];

	switch (action) {
	case ACTION_NONE:
		break;
	case ACTION_DUMP:
	case ACTION_DUMPHDRS:
		err = dump_image(filepath, opt_index, opt_name,
				 (action == ACTION_DUMPHDRS),
				 opt_raw_payload,
				 stdout);
		if (err)
			return 1;
		break;
	case ACTION_GETPAYLOAD:
		err = write_payload(filepath, opt_index, opt_name, stdout);
		if (err)
			return 1;
		break;
	}

	return 0;
}
