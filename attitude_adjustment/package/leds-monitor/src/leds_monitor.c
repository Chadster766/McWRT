/*
 * Copyright 2014 Belkin Inc.
 *
 * Author: Belkin Inc.
 *
 * This file is subject to the terms and conditions of version 2 of
 * the GNU General Public License.  See the file COPYING in the main
 * directory of this archive for more details.
 *
 */

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

//#define DEBUG
#ifdef DEBUG
#define mamba_dbg(fmt, arg...) printf("mamba_apps:%s " fmt "\n", __func__ , ## arg)
#else
#define mamba_dbg(fmt, arg...)
#endif

#define MAX_MONITOR 	50 /* 50s */

/* When a SIGUSR1 signal arrives, set this variable. */
volatile sig_atomic_t usr_interrupt = 0;
enum{
	LED_STOP=0,
	LED_START,
};
static int led_mode = LED_STOP;
static pthread_t pid;

void *handler_led(void *arg)
{
	int i = 0, blink = 0;
	struct timeval start, end;
	long mtime, secs, usecs;    

	gettimeofday(&start, NULL);
	while((led_mode == LED_START) && (i < MAX_MONITOR)) {
		sleep(1);
		i++;
		gettimeofday(&end, NULL);
		secs  = end.tv_sec  - start.tv_sec;
		usecs = end.tv_usec - start.tv_usec;
		mtime = ((secs) * 1000 + usecs/1000.0) + 0.5;

		if (secs > 1 && blink == 0) {
			blink = 1;
			mamba_dbg ("Blink 700ms/700ms\n");
			system("ledctrl power 255 700 700");
		}
		else if (secs > 10 && blink == 1) {
			blink = 2;
			mamba_dbg ("Blink 500ms/500ms\n");
			system("ledctrl power 255 500 500");
		}
	}
	mamba_dbg("Stop blink \n");
	system("ledctrl power off");
}

void handler_led_blink_reset (int sig)
{
	int err;

	if (led_mode == LED_STOP) {
		mamba_dbg ("Send signal start.\n");
		led_mode = LED_START;
		err = pthread_create(&pid, NULL, &handler_led, NULL);
		if (err != 0)
			mamba_dbg ("\ncan't create thread :[%s]", strerror(err));
		else
			mamba_dbg ("\n Thread created successfully\n");
	}
	else if (led_mode == LED_START)
	{
		led_mode = LED_STOP;
		mamba_dbg ("Send signal stop.\n");
		system("ledctrl power off");
	}
}

void handler_led_advance (int sig)
{
	mamba_dbg ("%s\n", __func__);
	led_mode = LED_STOP;
}

void register_signal(int sig) 
{
	struct sigaction usr_action;
	sigset_t block_mask;

	/* Establish the signal handler. */
	sigfillset (&block_mask);
	if (sig == SIGUSR1)
		usr_action.sa_handler = handler_led_blink_reset;
	else if (sig == SIGUSR2)
		usr_action.sa_handler = handler_led_advance;
	else {
		usr_interrupt = 1;
		return;
	}
	usr_action.sa_mask = block_mask;
	usr_action.sa_flags = 0;
	sigaction (sig, &usr_action, NULL);
}

int main (int argc, char *argv)
{
	register_signal(SIGUSR1);
	register_signal(SIGUSR2);

	/* Busy wait for the child to send a signal. */
	while (!usr_interrupt){
		sleep(100000);
	}

	/* Now continue execution. */
	puts ("That's all, folks!");

	return 0;
}
