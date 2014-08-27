#ifndef __IWINFO_H_
#define __IWINFO_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <glob.h>
#include <ctype.h>
#include <dirent.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <net/if.h>
#include <errno.h>


#define IWINFO_BUFSIZE	24 * 1024
#define IWINFO_ESSID_MAX_SIZE	32

#define IWINFO_80211_A       (1 << 0)
#define IWINFO_80211_B       (1 << 1)
#define IWINFO_80211_G       (1 << 2)
#define IWINFO_80211_N       (1 << 3)

#define IWINFO_CIPHER_NONE   (1 << 0)
#define IWINFO_CIPHER_WEP40  (1 << 1)
#define IWINFO_CIPHER_TKIP   (1 << 2)
#define IWINFO_CIPHER_WRAP   (1 << 3)
#define IWINFO_CIPHER_CCMP   (1 << 4)
#define IWINFO_CIPHER_WEP104 (1 << 5)
#define IWINFO_CIPHER_AESOCB (1 << 6)
#define IWINFO_CIPHER_CKIP   (1 << 7)

#define IWINFO_KMGMT_NONE    (1 << 0)
#define IWINFO_KMGMT_8021x   (1 << 1)
#define IWINFO_KMGMT_PSK     (1 << 2)

#define IWINFO_AUTH_OPEN     (1 << 0)
#define IWINFO_AUTH_SHARED   (1 << 1)

extern const char *IWINFO_CIPHER_NAMES[];
extern const char *IWINFO_KMGMT_NAMES[];
extern const char *IWINFO_AUTH_NAMES[];


enum iwinfo_opmode {
	IWINFO_OPMODE_UNKNOWN    = 0,
	IWINFO_OPMODE_MASTER     = 1,
	IWINFO_OPMODE_ADHOC      = 2,
	IWINFO_OPMODE_CLIENT     = 3,
	IWINFO_OPMODE_MONITOR    = 4,
	IWINFO_OPMODE_AP_VLAN    = 5,
	IWINFO_OPMODE_WDS        = 6,
	IWINFO_OPMODE_MESHPOINT  = 7,
	IWINFO_OPMODE_P2P_CLIENT = 8,
	IWINFO_OPMODE_P2P_GO     = 9,
};

extern const char *IWINFO_OPMODE_NAMES[];


struct iwinfo_rate_entry {
	uint32_t rate;
	int8_t mcs;
	uint8_t is_40mhz:1;
	uint8_t is_short_gi:1;
};

struct iwinfo_assoclist_entry {
	uint8_t	mac[6];
	int8_t signal;
	int8_t noise;
	uint32_t inactive;
	uint32_t rx_packets;
	uint32_t tx_packets;
	struct iwinfo_rate_entry rx_rate;
	struct iwinfo_rate_entry tx_rate;
};

struct iwinfo_txpwrlist_entry {
	uint8_t  dbm;
	uint16_t mw;
};

struct iwinfo_freqlist_entry {
	uint8_t channel;
	uint32_t mhz;
	uint8_t restricted;
};

struct iwinfo_crypto_entry {
	uint8_t	enabled;
	uint8_t wpa_version;
	uint8_t group_ciphers;
	uint8_t pair_ciphers;
	uint8_t auth_suites;
	uint8_t auth_algs;
};

struct iwinfo_scanlist_entry {
	uint8_t mac[6];
	uint8_t ssid[IWINFO_ESSID_MAX_SIZE+1];
	enum iwinfo_opmode mode;
	uint8_t channel;
	uint8_t signal;
	uint8_t quality;
	uint8_t quality_max;
	struct iwinfo_crypto_entry crypto;
};

struct iwinfo_country_entry {
	uint16_t iso3166;
	uint8_t ccode[4];
};

struct iwinfo_iso3166_label {
	uint16_t iso3166;
	uint8_t  name[28];
};

struct iwinfo_hardware_id {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_device_id;
};

struct iwinfo_hardware_entry {
	char vendor_name[64];
	char device_name[64];
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_device_id;
	int16_t txpower_offset;
	int16_t frequency_offset;
};

extern const struct iwinfo_iso3166_label IWINFO_ISO3166_NAMES[];

#define IWINFO_HARDWARE_FILE	"/usr/share/libiwinfo/hardware.txt"


struct iwinfo_ops {
	int (*mode)(const char *, int *);
	int (*channel)(const char *, int *);
	int (*frequency)(const char *, int *);
	int (*frequency_offset)(const char *, int *);
	int (*txpower)(const char *, int *);
	int (*txpower_offset)(const char *, int *);
	int (*bitrate)(const char *, int *);
	int (*signal)(const char *, int *);
	int (*noise)(const char *, int *);
	int (*quality)(const char *, int *);
	int (*quality_max)(const char *, int *);
	int (*mbssid_support)(const char *, int *);
	int (*hwmodelist)(const char *, int *);
	int (*ssid)(const char *, char *);
	int (*bssid)(const char *, char *);
	int (*country)(const char *, char *);
	int (*hardware_id)(const char *, char *);
	int (*hardware_name)(const char *, char *);
	int (*encryption)(const char *, char *);
	int (*assoclist)(const char *, char *, int *);
	int (*txpwrlist)(const char *, char *, int *);
	int (*scanlist)(const char *, char *, int *);
	int (*freqlist)(const char *, char *, int *);
	int (*countrylist)(const char *, char *, int *);
	void (*close)(void);
};

const char * iwinfo_type(const char *ifname);
const struct iwinfo_ops * iwinfo_backend(const char *ifname);
void iwinfo_finish(void);

#include "iwinfo/wext.h"

#ifdef USE_WL
#include "iwinfo/wl.h"
#endif

#ifdef USE_MADWIFI
#include "iwinfo/madwifi.h"
#endif

#ifdef USE_NL80211
#include "iwinfo/nl80211.h"
#endif

#endif
