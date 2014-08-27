/*
 *	wattsup - Program for controlling the Watts Up? Pro Device
 *
 *
 *	Copyright (c) 2005 Patrick Mochel
 *
 *	This program is released under the GPLv2
 *
 *
 *	Compiled with:
 *
 *	gcc -O2 -Wall -o wattsup wattsup.c
 *
 */

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<termios.h>
#include<ctype.h>
#include<getopt.h>
#include<signal.h>
#include<time.h>

#include<sys/stat.h>

static const char * wu_version = "0.02";


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

static const char * prog_name = "wattsup";

static const char * sysfs_path_start = "/sys/class/tty";

static char * wu_device = "ttyUSB0";
static int wu_fd = 0;
static int wu_count = 0;
static int wu_debug = 0;
static char *wu_delim = ", ";
static int wu_final = 0;
static int wu_interval = 1;
static int wu_label = 0;
static int wu_newline = 0;
static int wu_suppress = 0;

static int wu_localtime = 0;
static int wu_gmtime = 0;

static int wu_info_all = 0;
static int wu_no_data = 0;
static int wu_set_only = 0;

#define wu_strlen	256
#define wu_num_fields	18
#define wu_param_len	16

struct wu_packet {
	unsigned int	cmd;
	unsigned int	sub_cmd;
	unsigned int	count;
	char	buf[wu_strlen];
	int	len;
	char	* field[wu_num_fields];
	char	* label[wu_num_fields];
};


struct wu_data {
	unsigned int	watts;
	unsigned int	volts;
	unsigned int	amps;
	unsigned int	watt_hours;

	unsigned int	cost;
	unsigned int	mo_kWh;
	unsigned int	mo_cost;
	unsigned int	max_watts;

	unsigned int	max_volts;
	unsigned int	max_amps;
	unsigned int	min_watts;
	unsigned int	min_volts;

	unsigned int	min_amps;
	unsigned int	power_factor;
	unsigned int	duty_cycle;
	unsigned int	power_cycle;
};

struct wu_options {
	char	* longopt;
	int	shortopt;
	int	param;
	int	flag;
	char	* value;

	char	* descr;
	char	* option;
	char	* format;

	int	(*show)(int dev_fd);
	int	(*store)(int dev_fd);
};

enum {
	wu_option_help = 0,
	wu_option_version,

	wu_option_debug,

	wu_option_count,
	wu_option_final,

	wu_option_delim,
	wu_option_newline,
	wu_option_localtime,
	wu_option_gmtime,
	wu_option_label,

	wu_option_suppress,

	wu_option_cal,
	wu_option_header,

	wu_option_interval,
	wu_option_mode,
	wu_option_user,

	wu_option_info_all,
	wu_option_no_data,
	wu_option_set_only,
};


static char * wu_option_value(unsigned int index);


enum {
	wu_field_watts		= 0,
	wu_field_volts,
	wu_field_amps,

	wu_field_watt_hours,
	wu_field_cost,
	wu_field_mo_kwh,
	wu_field_mo_cost,

	wu_field_max_watts,
	wu_field_max_volts,
	wu_field_max_amps,

	wu_field_min_watts,
	wu_field_min_volts,
	wu_field_min_amps,

	wu_field_power_factor,
	wu_field_duty_cycle,
	wu_field_power_cycle,
};

struct wu_field {
	unsigned int	enable;
	char		* name;
	char		* descr;
};

static struct wu_field wu_fields[wu_num_fields] = {
	[wu_field_watts]	= {
		.name	= "watts",
		.descr	= "Watt Consumption",
	},

	[wu_field_min_watts]	= {
		.name	= "min-watts",
		.descr	= "Minimum Watts Consumed",
	},

	[wu_field_max_watts]	= {
		.name	= "max-watts",
		.descr	= "Maxium Watts Consumed",
	},

	[wu_field_volts]	= {
		.name	= "volts",
		.descr	= "Volts Consumption",
	},

	[wu_field_min_volts]	= {
		.name	= "max-volts",
		.descr	= "Minimum Volts Consumed",
	},

	[wu_field_max_volts]	= {
		.name	= "min-volts",
		.descr	= "Maximum Volts Consumed",
	},

	[wu_field_amps]		= {
		.name	= "amps",
		.descr	= "Amp Consumption",
	},

	[wu_field_min_amps]	= {
		.name	= "min-amps",
		.descr	= "Minimum Amps Consumed",
	},

	[wu_field_max_amps]	= {
		.name	= "max-amps",
		.descr	= "Maximum Amps Consumed",
	},

	[wu_field_watt_hours]	= {
		.name	= "kwh",
		.descr	= "Average KWH",
	},

	[wu_field_mo_kwh]	= {
		.name	= "mo-kwh",
		.descr	= "Average monthly KWH",
	},

	[wu_field_cost]		= {
		.name	= "cost",
		.descr	= "Cost per watt",
	},

	[wu_field_mo_cost]	= {
		.name	= "mo-cost",
		.descr	= "Monthly Cost",
	},

	[wu_field_power_factor]	= {
		.name	= "power-factor",
		.descr	= "Ratio of Watts vs. Volt Amps",
	},

	[wu_field_duty_cycle]	= {
		.name	= "duty-cycle",
		.descr	= "Percent of the Time On vs. Time Off",
	},

	[wu_field_power_cycle]	= {
		.name	= "power-cycle",
		.descr	= "Indication of power cycle",
	},

};



static void msg_start(const char * fmt, ...)
{
	va_list(ap);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

static void msg_end(void)
{
	printf("\n");
}

static void msg(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

static void dbg(const char * fmt, ...)
{
	va_list ap;

	if (wu_debug) {
		va_start(ap, fmt);
		msg_start("%s: [debug] ", prog_name);
		vprintf(fmt, ap);
		msg_end();
		va_end(ap);
	}
}

static void err(const char * fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	fprintf(stderr, "%s: [error] ", prog_name);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);
}

static void perr(const char * fmt, ...)
{
	char buf[1024];
	int n;
	va_list ap;

	va_start(ap, fmt);
	n = sprintf(buf, "%s: [error] ", prog_name);
	vsnprintf(buf + n, sizeof(buf) - n, fmt, ap);
	perror(buf);
	va_end(ap);
}

static int ret_err(int err)
{
	errno = err;
	return -1;
}


static void print_packet(struct wu_packet * p, char * str)
{
	int i;

	if (!wu_suppress)
		msg_start("Watts Up? %s\n", str);
	for (i = 0; i<  p->count; i++) {
		if (i)
			msg("%s", wu_newline ? "\n" : wu_delim);
		if (wu_label)
			msg("[%s] ", p->label[i]);
		msg(p->field[i]);
	}
	msg_end();
}


static void print_time(void)
{
	time_t t;
	struct tm * tm;

	if (wu_localtime || wu_gmtime) {
		time(&t);

		if (wu_localtime)
			tm = localtime(&t);
		else
			tm = gmtime(&t);

		msg("[%02d:%02d:%02d] ",
		    tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
}

static void print_packet_filter(struct wu_packet * p,
				int (*filter_ok)(struct wu_packet * p, int i, char * str))
{
	char buf[256];
	int printed;
	int i;

	print_time();
	for (i = 0, printed = 0; i<  p->count; i++) {
		if (!filter_ok(p, i, buf))
			continue;

		if (printed++)
			msg("%s", wu_newline ? "\n" : wu_delim);
		if (wu_label)
			msg("[%s] ", p->label[i]);
		msg(buf);
	}
	msg_end();
}


/*
 * Device should be something like "ttyS0"
 */

static int open_device(char * device_name, int * dev_fd)
{
	struct stat s;
	int ret;
	int cur_fd;

	cur_fd = open(".", O_RDONLY);
	if (cur_fd<  0) {
		perr("Could not open current directory.");
		return cur_fd;
	}

	ret = chdir(sysfs_path_start);
	if (ret) {
		perr(sysfs_path_start);
		return ret;
	}

	/*
	 * First, check if /sys/class/tty/<name>/ exists.
	 */

	dbg("Checking sysfs path: %s/%s", sysfs_path_start, device_name);

	ret = stat(device_name,&s);
	if (ret<  0) {
		perr(device_name);
		goto Done;
	}

	if (!S_ISDIR(s.st_mode)) {
		errno = -ENOTDIR;
		err("%s is not a TTY device.", device_name);
		goto Done;
	}

	dbg("%s is a registered TTY device", device_name);

	fchdir(cur_fd);


	/*
	 * Check if device node exists and is writable
	 */
	chdir("/dev");

	ret = stat(device_name,&s);
	if (ret<  0) {
		perr("/dev/%s (device node)", device_name);
		goto Done;
	}

	if (!S_ISCHR(s.st_mode)) {
		errno = -ENOTTY;
		ret = -1;
		err("%s is not a TTY character device.", device_name);
		goto Done;
	}

	dbg("%s has a device node", device_name);

	ret = access(device_name, R_OK | W_OK);
	if (ret) {
		perr("%s: Not writable?", device_name);
		goto Done;
	}

	ret = open(device_name, O_RDWR | O_NONBLOCK);
	if (ret<  0) {
		perr("Could not open %s");
		goto Done;
	}
	*dev_fd = ret;
	ret = 0;
Done:
	fchdir(cur_fd);
	close(cur_fd);
	return ret;
}


static int setup_serial_device(int dev_fd)
{
	struct termios t;
	int ret;

	ret = tcgetattr(dev_fd,&t);
	if (ret)
		return ret;

	cfmakeraw(&t);
	cfsetispeed(&t, B115200);
	cfsetospeed(&t, B115200);
	tcflush(dev_fd, TCIFLUSH);

	t.c_iflag |= IGNPAR;
	t.c_cflag&= ~CSTOPB;
	ret = tcsetattr(dev_fd, TCSANOW,&t);

	if (ret) {
		perr("setting terminal attributes");
		return ret;
	}

	return 0;
}


static int wu_write(int fd, struct wu_packet * p)
{
	int ret;
	int n;
	int i;
	char * s = p->buf;

	memset(p->buf, 0, sizeof(p->buf));
	n = sprintf(p->buf, "#%c,%c,%d", p->cmd, p->sub_cmd, p->count);
	p->len = n;
	s = p->buf + n;

	for (i = 0; i<  p->count; i++) {
		if ((p->len + strlen(p->field[i]) + 4)>= sizeof(p->buf)) {
			err("Overflowed command string");
			return ret_err(EOVERFLOW);
		}
		n = sprintf(s, ",%s", p->field[i]);
		s += n;
		p->len += n;
	}
	p->buf[p->len++] = ';';

	dbg("Writing '%s' (strlen = %d) (len = %d) to device",
	    p->buf, strlen(p->buf), p->len);
	ret = write(fd, p->buf, p->len);
	if (ret != p->len)
		perr("Writing to device");
	
	return ret>= 0 ? 0 : ret;
}


static void dump_packet(struct wu_packet * p)
{
	int i;

	dbg("Packet - Command '%c' %d parameters", p->cmd, p->count);
	
	for (i = 0; i<  p->count; i++)
		dbg("[%2d] [%20s] = \"%s\"", i, p->label[i], p->field[i]);
}


static int parse_packet(struct wu_packet * p)
{
	char * s, *next;
	int i;

	p->buf[p->len] = '\0';

	dbg("Parsing Packet, Raw buffer is (%d bytes) [%s]",
	    p->len, p->buf);

	s = p->buf;

	/*
	 * First character should be '#'
	 */
	if (s) {
		s = strchr(s, '#');
		if (s)
			s++;
		else {
			dbg("Invalid packet");
			return ret_err(EFAULT);
		}
	} else {
		dbg("Invalid packet");
		return ret_err(EFAULT);
	}

	/*
	 * Command character is first
	 */
	next = strchr(s, ',');
	if (next) {
		p->cmd = *s;
		s = ++next;
	} else {
		dbg("Invalid Command field [%s]", s);
		return ret_err(EFAULT);
	}

	/*
	 * Next character is the subcommand, and should be '-'
	 * Though, it doesn't matter, because we just
	 * discard it anyway.
	 */
	next = strchr(s, ',');
	if (next) {
		p->sub_cmd = *s;
		s = ++next;
	} else {
		dbg("Invalid 2nd field");
		return ret_err(EFAULT);
	}

	/*
	 * Next is the number of parameters,
	 * which should always be>  0.
	 */
	next = strchr(s, ',');
	if (next) {
		*next++ = '\0';
		p->count = atoi(s);
		s = next;
	} else {
		dbg("Couldn't determine number of parameters");
		return ret_err(EFAULT);
	}
	
	dbg("Have %d parameter%s (cmd = '%c')",
	    p->count, p->count>  1 ? "s" : "", p->cmd);

	/*
	 * Now, we loop over the rest of the string,
	 * storing a pointer to each in p->field[].
	 *
	 * The last character was originally a ';', but may have been
	 * overwritten with a '\0', so we make sure to catch
	 * that when converting the last parameter.
	 */
	for (i = 0; i<  p->count; i++) {
		next = strpbrk(s, ",;");
		if (next) {
			*next++ = '\0';
		} else {
			if (i<  (p->count - 1)) {
				dbg("Malformed parameter string [%s]", s);
				return ret_err(EFAULT);
			}
		}

		/*
		 * Skip leading white space in fields
		 */
		while (isspace(*s))
			s++;
		p->field[i] = s;
		s = next;
	}
	dump_packet(p);
	return 0;
}


static int wu_read(int fd, struct wu_packet * p)
{
	fd_set read_fd;
	struct timeval tv;
	int ret;

	FD_ZERO(&read_fd);
	FD_SET(fd,&read_fd);
	
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	ret = select(fd + 1,&read_fd, NULL, NULL,&tv);
	if (ret<  0) {
		perr("select on terminal device");
		return ret;
	} else if (ret>  0) {

		ret = read(fd, p->buf, wu_strlen);
		if (ret<  0) {
			perr("Reading from device");
			return ret;
		}
		p->len = ret;
	} else {
		dbg("Device timed out while reading");
		return ret_err(ETIME);
	}
	return parse_packet(p);
}


static int wu_show_header(int fd)
{
	struct wu_packet p = {
		.cmd		= 'H',
		.sub_cmd	= 'R',
		.count		= 0,
		.label = {
			[0] = "watts header",
			[1] = "volts header",
			[2] = "amps header",
			[3] = "kWh header",
			[4] = "cost header",
			[5] = "mo. kWh header",
			[6] = "mo. cost header",
			[7] = "max watts header",
			[8] = "max volts header",
			[9] = "max amps header",
			[10] = "min watts header",
			[11] = "min volts header",
			[12] = "min amps header",
			[13] = "power factor header",
			[14] = "duty cycle header",
			[15] = "power cycle header",
		}
	};
	int ret;

	ret = wu_write(fd,&p);
	if (ret) {
		perr("Requesting header strings");
		return ret;
	}
	sleep(1);

	ret = wu_read(fd,&p);
	if (ret) {
		perr("Reading header strings");
		return ret;
	}

	print_packet(&p, "Header Record");

	return 0;
}


static int wu_show_cal(int fd)
{
	struct wu_packet p = {
		.cmd		= 'F',
		.sub_cmd	= 'R',
		.count		= 0,
		.label = {
			[0] = "flags",
			[1] = "sample count",
			[2] = "volts gain",
			[3] = "volts bias",
			[4] = "amps gain",
			[5] = "amps bias",
			[6] = "amps offset",
			[7] = "low amps gain",
			[8] = "low amps bias",
			[9] = "low amps offset",
			[10] = "watts gain",
			[11] = "watts offset",
			[12] = "low watts gain",
			[13] = "low watts offset",
		},
	};
	int ret;

	ret = wu_write(fd,&p);
	if (ret) {
		perr("Requesting calibration parameters");
		return ret;
	}
	sleep(1);

	ret = wu_read(fd,&p);
	if (ret) {
		perr("Reading header strings");
		return ret;
	}
	print_packet(&p, "Calibration Settings");

	return 0;
}

static int wu_start_log(void)
{
	struct wu_packet p = {
		.cmd		= 'L',
		.sub_cmd	= 'W',
		.count		= 3,
		.field		= {
			[0] = "E",
			[1] = "1",
			[2] = "1",
		},
	};
	int ret;

	/*
	 * Start up logging
	 */
	ret = wu_write(wu_fd,&p);
	if (!ret)
		sleep(1);
	else {
		perr("Starting External Logging");
		return ret;
	}
	return ret;
}

static int wu_stop_log(void)
{
	struct wu_packet p = {
		.cmd		= 'L',
		.sub_cmd	= 'R',
		.count		= 0,
		.label = {
			[0] = "time stamp",
			[1] = "interval",
		},
	};
	int ret;

	/*
	 * Stop logging and read time stamp.
	 */
	ret = wu_write(wu_fd,&p);
	if (ret) {
		perr("Stopping External Logging");
		return ret;
	}
	sleep(1);

	ret = wu_read(wu_fd,&p);
	if (ret) {
		perr("Reading final time stamp");
		return ret;
	}
	if (wu_final)
		print_packet(&p, "Final Time Stamp and Interval");
	return ret;
}

static int filter_data(struct wu_packet * p, int i, char * buf)
{
	if (i<  wu_num_fields) {
		if (wu_fields[i].enable) {
			double val = strtod(p->field[i], NULL);
			snprintf(buf, 256, "%.1f", val / 10.0);
			return 1;
		}
	}
	return 0;
}

static int wu_clear(int fd)
{
	struct wu_packet p = {
		.cmd		= 'R',
		.sub_cmd	= 'W',
		.count		= 0,
	};
	int ret;

	/*
	 * Clear the memory
	 */
	ret = wu_write(fd,&p);
	if (ret)
		perr("Clearing memory");
	else
		sleep(2);
	
	/*
	 * Dummy read
	 */
	wu_read(fd,&p);
	return ret;

}

static int wu_read_data(int fd)
{
	struct wu_packet p = {
		.label = {
			[0] = "watts",
			[1] = "volts",
			[2] = "amps",
			[3] = "watt hours",
			[4] = "cost",
			[5] = "mo. kWh",
			[6] = "mo. cost",
			[7] = "max watts",
			[8] = "max volts",
			[9] = "max amps",
			[10] = "min watts",
			[11] = "min volts",
			[12] = "min amps",
			[13] = "power factor",
			[14] = "duty cycle",
			[15] = "power cycle",
		},
	};
	int num_read = 0;
	int retry = 0;
	int ret;
	int i;

	static const int wu_max_retry = 2;

	i = 0;
	while (1) {
		
		ret = wu_read(fd,&p);
		if (ret) {
			if (++retry<  wu_max_retry) {
				dbg("Bad record back, retrying\n");
				sleep(wu_interval);
				continue;
			} else if (retry == wu_max_retry) {
				dbg("Still couldn't get a good record, resetting\n");
				wu_stop_log();
				wu_clear(fd);
				wu_start_log();
				num_read = 0;
				sleep(wu_interval);
				continue;
			}
			perr("Blech. Giving up on read");
			break;
		} else if (retry)
			retry = 0;

		dbg("[%d] ", num_read);
		num_read++;
		print_packet_filter(&p, filter_data);

		if (wu_count&&  (++i == wu_count))
			break;
		
		sleep(wu_interval);
	}
	return 0;

}


static int wu_show_interval(int fd)
{
	struct wu_packet p = {
		.cmd		= 'S',
		.sub_cmd	= 'R',
		.count		= 0,
		.label = {
			[0] = "reserved",
			[1] = "interval",
		},
	};
	int ret;
	
	ret = wu_write(fd,&p);
	if (ret) {
		perr("Requesting interval");
		return ret;
	}
	sleep(1);

	ret = wu_read(fd,&p);
	if (ret) {
		perr("Reading interval");
		return ret;
	}
	print_packet(&p, "Interval Settings");

	return 0;
}

static int wu_write_interval(int fd, unsigned int seconds,
			     unsigned int interval)
{
	char str_seconds[wu_param_len];
	char str_interval[wu_param_len];
	struct wu_packet p = {
		.cmd		= 'S',
		.sub_cmd	= 'W',
		.count		= 2,
		.field		= {
			[0] = str_seconds,
			[1] = str_interval,
		},
	};
	int ret;

	snprintf(str_seconds, wu_param_len, "%ud", seconds);
	snprintf(str_interval, wu_param_len, "%ud", interval);

	ret = wu_write(fd,&p);
	if (ret) {
		perr("Setting Sampling Interval");
		return ret;
	}
	sleep(1);
	return 0;
}

static int wu_store_interval(int fd)
{
	char * s = wu_option_value(wu_option_interval);
	char * end;

	wu_interval = strtol(s,&end, 0);
	if (*end) {
		err("Invalid interval: %s", s);
		return ret_err(EINVAL);
	}
	return wu_write_interval(fd, 1, wu_interval);
}

static int wu_show_mode(int fd)
{
	struct wu_packet p = {
		.cmd		= 'M',
		.sub_cmd	= 'R',
		.count		= 0,
		.label		= {
			[0] = "display mode",
		},
	};
	int ret;

	ret = wu_write(fd,&p);
	if (ret) {
		perr("Requesting device display mode");
		return ret;
	}

	ret = wu_read(fd,&p);
	if (ret) {
		perr("Reaing device display mode");
		return ret;
	}
	dump_packet(&p);
	return ret;
}

static int wu_write_mode(int fd, int mode)
{
	char str_mode[wu_param_len];
	struct wu_packet p = {
		.cmd		= 'M',
		.sub_cmd	= 'W',
		.count		= 1,
		.field		= {
			[0] = str_mode,
		},
	};
	int ret;
	
	snprintf(str_mode, wu_param_len, "%ud", mode);
	ret = wu_write(fd,&p);
	if (ret)
		perr("Setting device display mode");
	else
		sleep(1);
	return ret;
}

static int wu_store_mode(int fd)
{
	char * s = wu_option_value(wu_option_mode);
	char * end;
	unsigned int mode;

	mode = strtol(s,&end, 0);
	if (*end) {
		err("Invalid mode: %s", s);
		return ret_err(EINVAL);
	}
	return wu_write_mode(fd, mode);
}



static int wu_show_user(int fd)
{
	struct wu_packet p = {
		.cmd		= 'U',
		.sub_cmd	= 'R',
		.count		= 0,
		.label		= {
			[0] = "cost per kWh",
			[1] = "2nd tier cost",
			[2] = "2nd tier threshold",
			[3] = "duty cycle threshold",
		},
	};
	int ret;

	ret = wu_write(fd,&p);
	if (ret) {
		perr("Requesting user parameters");
		return ret;
	}
	sleep(1);

	ret = wu_read(fd,&p);
	if (ret) {
		perr("Reading user parameters");
		return ret;
	}
	print_packet(&p, "User Settings");
	return 0;
}


static int wu_write_user(int fd, unsigned int kwh_cost,
			 unsigned int second_tier_cost,
			 unsigned int second_tier_threshold,
			 unsigned int duty_cycle_threshold)
{
	char str_kwh_cost[wu_param_len];
	char str_2nd_tier_cost[wu_param_len];
	char str_2nd_tier_threshold[wu_param_len];
	char str_duty_cycle_threshold[wu_param_len];

	struct wu_packet p = {
		.cmd		= 'U',
		.sub_cmd	= 'R',
		.count		= 0,
		.label		= {
			[0] = str_kwh_cost,
			[1] = str_2nd_tier_cost,
			[2] = str_2nd_tier_threshold,
			[3] = str_duty_cycle_threshold,
		},
	};
	int ret;

	snprintf(str_kwh_cost, wu_param_len, "%ud", kwh_cost);
	snprintf(str_2nd_tier_cost, wu_param_len, "%ud",
		 second_tier_cost);
	snprintf(str_2nd_tier_threshold, wu_param_len, "%ud",
		 second_tier_threshold);
	snprintf(str_duty_cycle_threshold, wu_param_len, "%ud",
		 duty_cycle_threshold);

	ret = wu_write(fd,&p);
	if (ret)
		perr("Writing user parameters");
	else
		sleep(1);
	return ret;
}

static int wu_store_user(int fd)
{
	unsigned int kwh_cost;
	unsigned int second_tier_cost;
	unsigned int second_tier_threshold;
	unsigned int duty_cycle_threshold;
	char * buf = wu_option_value(wu_option_user);
	char * s = buf;
	char * next;

	if (!buf) {
		err("No user parameters?");
		return ret_err(EINVAL);
	}

	kwh_cost = strtoul(s,&next, 0);
	if (next == s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}

	s = next;
	while (s&&  !isdigit(*s))
		s++;
	if (!s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}


	second_tier_cost = strtoul(s,&next, 0);
	if (next == s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}

	s = next;
	while (s&&  !isdigit(*s))
		s++;
	if (!s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}


	second_tier_threshold = strtoul(s,&next, 0);
	if (next == s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}

	s = next;
	while (s&&  !isdigit(*s))
		s++;
	if (!s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}


	duty_cycle_threshold = strtoul(s,&next, 0);
	if (next == s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}

	s = next;
	while (s&&  !isdigit(*s))
		s++;
	if (!s) {
		err("Incomplete user parameters");
		return ret_err(EINVAL);
	}

	return wu_write_user(fd, kwh_cost, second_tier_cost,
			     second_tier_threshold, duty_cycle_threshold);
}


static void enable_field(char * name)
{
	int i;

	for (i = 0; i<  wu_num_fields; i++) {
		if (!strcasecmp(wu_fields[i].name, name)) {
			wu_fields[i].enable = 1;
			break;
		}
	}
}

static void enable_all_fields(void)
{
	int i;

	for (i = 0; i<  wu_num_fields; i++)
		wu_fields[i].enable = 1;
}



static int wu_show_help(int);
static int wu_show_version(int);



static int wu_store_count(int unused)
{
	char * s = wu_option_value(wu_option_count);
	char * end;

	if (s) {
		wu_count = strtol(s,&end, 0);
		if (*end) {
			err("Bad count field");
			return ret_err(EINVAL);
		}
	}
	return 0;
}

static int wu_store_debug(int unused)
{
	wu_debug = 1;
	return 0;
}

static int wu_store_delim(int unused)
{
	char * s = wu_option_value(wu_option_delim);
	
	if (s)
		wu_delim = s;
	return 0;
}

static int wu_store_final(int unused)
{
	wu_final = 1;
	return 0;
}

static int wu_store_label(int unused)
{
	wu_label = 1;
	return 0;
}

static int wu_store_newline(int unused)
{
	wu_newline = 1;
	return 0;
}

static int wu_store_suppress(int unused)
{
	wu_suppress = 1;
	return 0;
}

static int wu_store_localtime(int unused)
{
	wu_localtime = 1;
	return 0;
}

static int wu_store_gmtime(int unused)
{
	wu_gmtime = 1;
	return 0;
}

static int wu_store_info_all(int unused)
{
	wu_info_all = 1;
	return 0;
}

static int wu_store_no_data(int unused)
{
	wu_no_data = 1;
	return 0;
}

static int wu_store_set_only(int unused)
{
	wu_set_only = 1;
	return 0;
}


/**
 *	wu_options - command line options and their associated flags
 *
 */
static struct wu_options wu_options[] = {

	/*
	 * Help!
	 */
	[wu_option_help] = {
		.longopt	= "help",
		.shortopt	= 'h',
		.param		= 0,
		.descr		= "Display help text and exit",
		.show		= wu_show_help,
	},

	[wu_option_version] = {
		.longopt	= "version",
		.shortopt	= 'V',
		.param		= 0,
		.descr		= "Display version information and exit",
		.show		= wu_show_version,
	},

	/*
	 * Modifies the output for all other options
	 */
	[wu_option_debug] = {
		.longopt	= "debug",
		.shortopt	= 'd',
		.param		= 0,
		.descr		= "Print out debugging messages",
		.store		= wu_store_debug,
	},

	/*
	 * For data reading..
	 */
	[wu_option_count] = {
		.longopt	= "count",
		.shortopt	= 'c',
		.param		= 1,
		.descr		= "Specify number of data samples",
		.option		= "<n>",
		.store		= wu_store_count,
	},

	[wu_option_final] = {
		.longopt	= "final",
		.shortopt	= 'z',
		.param		= 0,
		.descr		= "Print final interval information",
		.store		= wu_store_final,
	},

	/*
	 * Modifies output for each option (most relevant for data)
	 */
	[wu_option_delim] = {
		.longopt	= "delim",
		.shortopt	= 'f',
		.param		= 1,
		.descr		= "Set field delimiter (default \", \")",
		.option		= "<str>",
		.store		= wu_store_delim,
	},

	[wu_option_newline] = {
		.longopt	= "newline",
		.shortopt	= 'n',
		.param		= 0,
		.descr		= "Use '\\n' as delimter instead",
		.store		= wu_store_newline,
	},

	[wu_option_localtime] = {
		.longopt	= "localtime",
		.shortopt	= 't',
		.param		= 0,
		.descr		= "Print localtime with each data reading",
		.store		= wu_store_localtime,
	},

	[wu_option_gmtime] = {
		.longopt	= "gmtime",
		.shortopt	= 'g',
		.param		= 0,
		.descr		= "Print GMT time with each data reading",
		.store		= wu_store_gmtime,
	},

	[wu_option_label] = {
		.longopt	= "label",
		.shortopt	= 'l',
		.param		= 0,
		.descr		= "Show labels of each field",
		.store		= wu_store_label,
	},

	/*
	 * Relevant for each of the fields below
	 */
	[wu_option_suppress] = {
		.longopt	= "suppress",
		.shortopt	= 's',
		.param		= 0,
		.descr		= "Suppress printing of the field description",
		.store		= wu_store_suppress,
	},

	/*
	 * These options print values from the device and exit.
	 */
	[wu_option_cal] = {
		.longopt	= "calibrate",
		.shortopt	= 'b',
		.param		= 0,
		.descr		= "Print calibration parameters",
		.show		= wu_show_cal,
	},

	[wu_option_header] = {
		.longopt	= "header",
		.shortopt	= 'r',
		.param		= 0,
		.descr		= "Print data field names (as read from device)",
		.show		= wu_show_header,
	},

	/*
	 * These options have an optional parameter.
	 * W/o that parameter, they print values from the device.
	 * W/ that parameter, they set that option and read data.
	 *
	 * Except when the 'set-only' parameter is used, then the
	 * parameters are set, then re-read and printed.
	 */
	[wu_option_interval] = {
		.longopt	= "interval",
		.shortopt	= 'i',
		.param		= 2,
		.descr		= "Get/Set sampling interval",
		.option		= "<n>",
		.show		= wu_show_interval,
		.store		= wu_store_interval,
	},

	[wu_option_mode] = {
		.longopt	= "mode",
		.shortopt	= 'm',
		.param		= 2,
		.descr		= "Get/Set display mode",
		.option		= "<n>",
		.show		= wu_show_mode,
		.store		= wu_store_mode,
	},

	[wu_option_user] = {
		.longopt	= "user",
		.shortopt	= 'u',
		.param		= 2,
		.descr		= "Get/Set user parameters",
		.option		= "<str>",
		.format		= "<cost per kwh>,<2nd tier cost>,"
				  "<2nd tier threshold>,"
				  "<duty cycle threshold>",
		.show		= wu_show_user,
		.store		= wu_store_user,
	},

	[wu_option_info_all] = {
		.longopt	= "show-all",
		.shortopt	= 'a',
		.param		= 0,
		.descr		= "Show all device parameters",
		.store		= wu_store_info_all,
	},

	[wu_option_no_data] = {
		.longopt	= "no-data",
		.shortopt	= 'N',
		.param		= 0,
		.descr		= "Don't read any data (just read device info)",
		.store		= wu_store_no_data,
	},

	[wu_option_set_only] = {
		.longopt	= "set-only",
		.shortopt	= 'S',
		.param		= 0,
		.descr		= "Set parameters only (don't read them back)",
		.store		= wu_store_set_only,
	},
};

#define wu_num_options ARRAY_SIZE(wu_options)

static int wu_show_version(int unused)
{
	printf("%s Version %s\n", prog_name, wu_version);
	return 0;
}

static int wu_show_help(int unused)
{
	int i;
	int n;

	wu_show_version(unused);
	printf("  A program for interfacing with the Watts Up? Power Meter\n");
	printf("\n");

	printf("Usage: %s [<options>  ... ]<device>  [<values>  ... ]\n",
	       prog_name);
	printf("\n");
	
	printf("<device>  is the serial port the device is connected at.\n");
	printf("\n");

	printf("<options>  are any of the following:\n");
	for (i = 0; i<  wu_num_options; i++) {
		n = printf("  -%c", wu_options[i].shortopt);

		if (wu_options[i].param == 0)
			n = printf(" ");
		else if (wu_options[i].param == 1)
			n = printf(" %s", wu_options[i].option);
		else if (wu_options[i].param == 2)
			n = printf(" [%s]", wu_options[i].option);

		n += printf("%*c| ", n - 12, ' ');
		n += printf("--%s", wu_options[i].longopt);

		if (wu_options[i].param == 0)
			n += printf(" ");
		else if (wu_options[i].param == 1)
			n += printf("=%s", wu_options[i].option);
		else if (wu_options[i].param == 2)
			n += printf("[=%s]", wu_options[i].option);

		printf("%*c%s\n",
		       40 - n, ' ', wu_options[i].descr);
	}
	printf("\n");
	printf("<value>  specifies which of these to print out (default: ALL)\n");
	for (i = 0; i<  wu_num_fields; i++) {
		printf("%12s -- %s\n", wu_fields[i].name, wu_fields[i].descr);
	}
	printf("\n");

	return 0;
}


static char * wu_option_value(unsigned int index)
{
	return (index<  wu_num_options) ? wu_options[index].value : NULL;
}


static int wu_check_option_show(int index)
{
	/*
	 * Return 1 if we need to print something out for
	 * a particular option.
	 */
	if (index<  wu_num_options) {
		if (wu_options[index].flag) {
			return 1;
		}
	}
	return 0;
}

static int wu_check_option_store(int index)
{
	/*
	 * Return a 1 if this option is set.
	 */

	if (index<  wu_num_options) {
		if (wu_options[index].flag) {
			return 1;
		}
	}
	return 0;
}


static int wu_show(int index, int dev_fd)
{
	if (wu_options[index].show)
		return wu_options[index].show(dev_fd);
	return 0;
}

/*
 * Check if the option is set, and call its method if so.
 * Return whether or not we did anything..
 */

static int wu_check_show(int index, int dev_fd)
{
	if (wu_check_option_show(index)) {
		wu_show(index, dev_fd);
		return 1;
	}
	return 0;
}


/*
 * Check if the option is set and if so, call it
 * Return the value from the ->store() method.
 */

static int wu_check_store(int index, int dev_fd)
{
	if (wu_check_option_store(index)) {
		if (wu_options[index].store)
			return wu_options[index].store(dev_fd);
	}
	return 0;
}


static void make_longopt(struct option * l)
{
	int i;

	for (i = 0; i<  wu_num_options; i++) {
		l[i].name = wu_options[i].longopt;
		l[i].has_arg = wu_options[i].param;
		l[i].flag =&wu_options[i].flag;
		l[i].val = 0;
	}
}

static void make_shortopt(char * str)
{
	int i;
	char * s = str;
	
	for (i = 0; i<  wu_num_options; i++) {
		*s++ = wu_options[i].shortopt;
		if (wu_options[i].param)
			*s++ = wu_options[i].param == 1 ? ':' : ';';
	}
}

static void enable_short_option(int c, char * arg)
{
	int i;

	/*
	 * Friggin' getopt_long() will return the
	 * character if we get a short option (e.g. '-h'),
	 * instead of returning 0 like it does when it
	 * gets a long option (e.g. "--help"). Ugh.
	 */
	for (i = 0; i<  wu_num_options; i++) {
		if (wu_options[i].shortopt == c) {
			wu_options[i].flag = 1;
			if (arg)
				wu_options[i].value = strdup(arg);
			break;
		}
	}
}

static int parse_args(int argc, char ** argv)
{
	struct option longopts[wu_num_options + 1] = { };
	char shortopts[wu_num_options * 2] = "";

	make_longopt(longopts);
	make_shortopt(shortopts);


	while (1) {
		int c;
		int index;

		c = getopt_long(argc, argv, shortopts,
				longopts,&index);
		if (c == -1)
			break;
		
		switch (c) {
		case 0:
			wu_options[index].flag = 1;
			if (optarg)
				wu_options[index].value = strdup(optarg);
			
			printf("long option: val = %c, optarg = %s\n",
			       wu_options[index].shortopt, optarg);
			break;
		case '?':
			err("Bad parameter");
			return ret_err(EINVAL);
			break;
		default:
			enable_short_option(c, optarg);
			break;
		}
	}

	/*
	 * Check for help request now and bail after
	 * printing it, if it's set.
	 */
	if (wu_check_show(wu_option_help, 0))
		exit(0);

	if (wu_check_show(wu_option_version, 0))
		exit(0);

	/*
	 * Fields to print out
	 */
	if (optind<  argc) {
		int i;

		wu_device = argv[optind++];

		if (optind<  argc) {
			for (i = optind; i<  argc; i++)
				enable_field(argv[i]);
		} else
			enable_all_fields();

	} else {
		wu_show(wu_option_help, 0);
		return ret_err(EINVAL);
	}
	return 0;
}


int main(int argc, char ** argv)
{
	int ret;
	int fd = 0;

	ret = parse_args(argc, argv);
	if (ret)
		return 0;

	/*
	 * Try to enable debugging early
	 */
	if ((ret = wu_check_store(wu_option_debug, 0)))
		goto Close;

	ret = open_device(wu_device,&fd);
	if (ret)
		return ret;

	dbg("%s: Open for business", wu_device);

	ret = setup_serial_device(fd);
	if (ret)
		goto Close;

	wu_clear(fd);

	wu_fd = fd;

	/*
	 * Set delimeter before we print out any fields.
	 */
	if ((ret = wu_check_store(wu_option_delim, fd)))
		goto Close;

	/*
	 * Ditto for 'label' and 'newline' flags.
	 */
	if ((ret = wu_check_store(wu_option_label, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_newline, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_suppress, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_localtime, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_gmtime, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_set_only, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_no_data, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_info_all, fd)))
		goto Close;


	/*
	 * Options to set device parameters.
	 */
	if ((ret = wu_check_store(wu_option_interval, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_mode, fd)))
		goto Close;

	if ((ret = wu_check_store(wu_option_user, fd)))
		goto Close;

	/*
	 * Check for options to print device info
	 */
	if (wu_info_all) {
		wu_show(wu_option_cal, fd);
		wu_show(wu_option_header, fd);
		wu_show(wu_option_interval, fd);
		wu_show(wu_option_mode, fd);
		wu_show(wu_option_user, fd);
	} else {
		wu_check_show(wu_option_cal, fd);
		wu_check_show(wu_option_header, fd);

		if (!wu_set_only) {
			wu_check_show(wu_option_interval, fd);
			wu_check_show(wu_option_mode, fd);
			wu_check_show(wu_option_user, fd);
		}
	}

	if (!wu_no_data) {

		if ((ret = wu_check_store(wu_option_count, fd)))
			goto Close;

		if ((ret = wu_check_store(wu_option_final, fd)))
			goto Close;

		if ((ret = wu_start_log()))
			goto Close;
	
		wu_read_data(fd);
		
		wu_stop_log();
	}
Close:
	close(fd);
	return ret;
}


