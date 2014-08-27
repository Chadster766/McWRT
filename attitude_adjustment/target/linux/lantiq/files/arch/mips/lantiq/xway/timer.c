#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#include <asm/irq.h>
#include <asm/div64.h>
#include "../clk.h"

#include <lantiq_soc.h>
#include <lantiq_irq.h>
#include <lantiq_timer.h>

#define MAX_NUM_OF_32BIT_TIMER_BLOCKS	6

#ifdef TIMER1A
#define FIRST_TIMER			TIMER1A
#else
#define FIRST_TIMER			2
#endif

/*
 *  GPTC divider is set or not.
 */
#define GPTU_CLC_RMC_IS_SET		0

/*
 *  Timer Interrupt (IRQ)
 */
/*  Must be adjusted when ICU driver is available */
#define TIMER_INTERRUPT			(INT_NUM_IM3_IRL0 + 22)

/*
 *  Bits Operation
 */
#define GET_BITS(x, msb, lsb)		\
	(((x) & ((1 << ((msb) + 1)) - 1)) >> (lsb))
#define SET_BITS(x, msb, lsb, value)	\
	(((x) & ~(((1 << ((msb) + 1)) - 1) ^ ((1 << (lsb)) - 1))) | \
	(((value) & ((1 << (1 + (msb) - (lsb))) - 1)) << (lsb)))

/*
 *  GPTU Register Mapping
 */
#define LQ_GPTU			(KSEG1 + 0x1E100A00)
#define LQ_GPTU_CLC		((volatile u32 *)(LQ_GPTU + 0x0000))
#define LQ_GPTU_ID			((volatile u32 *)(LQ_GPTU + 0x0008))
#define LQ_GPTU_CON(n, X)		((volatile u32 *)(LQ_GPTU + 0x0010 + ((X) * 4) + ((n) - 1) * 0x0020))	/* X must be either A or B */
#define LQ_GPTU_RUN(n, X)		((volatile u32 *)(LQ_GPTU + 0x0018 + ((X) * 4) + ((n) - 1) * 0x0020))	/* X must be either A or B */
#define LQ_GPTU_RELOAD(n, X)	((volatile u32 *)(LQ_GPTU + 0x0020 + ((X) * 4) + ((n) - 1) * 0x0020))	/* X must be either A or B */
#define LQ_GPTU_COUNT(n, X)	((volatile u32 *)(LQ_GPTU + 0x0028 + ((X) * 4) + ((n) - 1) * 0x0020))	/* X must be either A or B */
#define LQ_GPTU_IRNEN		((volatile u32 *)(LQ_GPTU + 0x00F4))
#define LQ_GPTU_IRNICR		((volatile u32 *)(LQ_GPTU + 0x00F8))
#define LQ_GPTU_IRNCR		((volatile u32 *)(LQ_GPTU + 0x00FC))

/*
 *  Clock Control Register
 */
#define GPTU_CLC_SMC			GET_BITS(*LQ_GPTU_CLC, 23, 16)
#define GPTU_CLC_RMC			GET_BITS(*LQ_GPTU_CLC, 15, 8)
#define GPTU_CLC_FSOE			(*LQ_GPTU_CLC & (1 << 5))
#define GPTU_CLC_EDIS			(*LQ_GPTU_CLC & (1 << 3))
#define GPTU_CLC_SPEN			(*LQ_GPTU_CLC & (1 << 2))
#define GPTU_CLC_DISS			(*LQ_GPTU_CLC & (1 << 1))
#define GPTU_CLC_DISR			(*LQ_GPTU_CLC & (1 << 0))

#define GPTU_CLC_SMC_SET(value)		SET_BITS(0, 23, 16, (value))
#define GPTU_CLC_RMC_SET(value)		SET_BITS(0, 15, 8, (value))
#define GPTU_CLC_FSOE_SET(value)	((value) ? (1 << 5) : 0)
#define GPTU_CLC_SBWE_SET(value)	((value) ? (1 << 4) : 0)
#define GPTU_CLC_EDIS_SET(value)	((value) ? (1 << 3) : 0)
#define GPTU_CLC_SPEN_SET(value)	((value) ? (1 << 2) : 0)
#define GPTU_CLC_DISR_SET(value)	((value) ? (1 << 0) : 0)

/*
 *  ID Register
 */
#define GPTU_ID_ID			GET_BITS(*LQ_GPTU_ID, 15, 8)
#define GPTU_ID_CFG			GET_BITS(*LQ_GPTU_ID, 7, 5)
#define GPTU_ID_REV			GET_BITS(*LQ_GPTU_ID, 4, 0)

/*
 *  Control Register of Timer/Counter nX
 *    n is the index of block (1 based index)
 *    X is either A or B
 */
#define GPTU_CON_SRC_EG(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 10))
#define GPTU_CON_SRC_EXT(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 9))
#define GPTU_CON_SYNC(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 8))
#define GPTU_CON_EDGE(n, X)		GET_BITS(*LQ_GPTU_CON(n, X), 7, 6)
#define GPTU_CON_INV(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 5))
#define GPTU_CON_EXT(n, X)		(*LQ_GPTU_CON(n, A) & (1 << 4))	/* Timer/Counter B does not have this bit */
#define GPTU_CON_STP(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 3))
#define GPTU_CON_CNT(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 2))
#define GPTU_CON_DIR(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 1))
#define GPTU_CON_EN(n, X)		(*LQ_GPTU_CON(n, X) & (1 << 0))

#define GPTU_CON_SRC_EG_SET(value)	((value) ? 0 : (1 << 10))
#define GPTU_CON_SRC_EXT_SET(value)	((value) ? (1 << 9) : 0)
#define GPTU_CON_SYNC_SET(value)	((value) ? (1 << 8) : 0)
#define GPTU_CON_EDGE_SET(value)	SET_BITS(0, 7, 6, (value))
#define GPTU_CON_INV_SET(value)		((value) ? (1 << 5) : 0)
#define GPTU_CON_EXT_SET(value)		((value) ? (1 << 4) : 0)
#define GPTU_CON_STP_SET(value)		((value) ? (1 << 3) : 0)
#define GPTU_CON_CNT_SET(value)		((value) ? (1 << 2) : 0)
#define GPTU_CON_DIR_SET(value)		((value) ? (1 << 1) : 0)

#define GPTU_RUN_RL_SET(value)		((value) ? (1 << 2) : 0)
#define GPTU_RUN_CEN_SET(value)		((value) ? (1 << 1) : 0)
#define GPTU_RUN_SEN_SET(value)		((value) ? (1 << 0) : 0)

#define GPTU_IRNEN_TC_SET(n, X, value)	((value) ? (1 << (((n) - 1) * 2 + (X))) : 0)
#define GPTU_IRNCR_TC_SET(n, X, value)	((value) ? (1 << (((n) - 1) * 2 + (X))) : 0)

#define TIMER_FLAG_MASK_SIZE(x)		(x & 0x0001)
#define TIMER_FLAG_MASK_TYPE(x)		(x & 0x0002)
#define TIMER_FLAG_MASK_STOP(x)		(x & 0x0004)
#define TIMER_FLAG_MASK_DIR(x)		(x & 0x0008)
#define TIMER_FLAG_NONE_EDGE		0x0000
#define TIMER_FLAG_MASK_EDGE(x)		(x & 0x0030)
#define TIMER_FLAG_REAL			0x0000
#define TIMER_FLAG_INVERT		0x0040
#define TIMER_FLAG_MASK_INVERT(x)	(x & 0x0040)
#define TIMER_FLAG_MASK_TRIGGER(x)	(x & 0x0070)
#define TIMER_FLAG_MASK_SYNC(x)		(x & 0x0080)
#define TIMER_FLAG_CALLBACK_IN_HB	0x0200
#define TIMER_FLAG_MASK_HANDLE(x)	(x & 0x0300)
#define TIMER_FLAG_MASK_SRC(x)		(x & 0x1000)

struct timer_dev_timer {
	unsigned int f_irq_on;
	unsigned int irq;
	unsigned int flag;
	unsigned long arg1;
	unsigned long arg2;
};

struct timer_dev {
	struct mutex gptu_mutex;
	unsigned int number_of_timers;
	unsigned int occupation;
	unsigned int f_gptu_on;
	struct timer_dev_timer timer[MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2];
};

unsigned long ltq_danube_fpi_bus_clock(int fpi);
unsigned long ltq_vr9_fpi_bus_clock(int fpi);

unsigned int ltq_get_fpi_bus_clock(int fpi) {
	if (ltq_is_ase())
		return CLOCK_133M;
	else if (ltq_is_vr9())
		return ltq_vr9_fpi_bus_clock(fpi);

	return ltq_danube_fpi_bus_clock(fpi);
}


static long gptu_ioctl(struct file *, unsigned int, unsigned long);
static int gptu_open(struct inode *, struct file *);
static int gptu_release(struct inode *, struct file *);

static struct file_operations gptu_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = gptu_ioctl,
	.open = gptu_open,
	.release = gptu_release
};

static struct miscdevice gptu_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "gptu",
	.fops = &gptu_fops,
};

static struct timer_dev timer_dev;

static irqreturn_t timer_irq_handler(int irq, void *p)
{
	unsigned int timer;
	unsigned int flag;
	struct timer_dev_timer *dev_timer = (struct timer_dev_timer *)p;

	timer = irq - TIMER_INTERRUPT;
	if (timer < timer_dev.number_of_timers
		&& dev_timer == &timer_dev.timer[timer]) {
		/*  Clear interrupt.    */
		ltq_w32(1 << timer, LQ_GPTU_IRNCR);

		/*  Call user hanler or signal. */
		flag = dev_timer->flag;
		if (!(timer & 0x01)
			|| TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT) {
			/* 16-bit timer or timer A of 32-bit timer  */
			switch (TIMER_FLAG_MASK_HANDLE(flag)) {
			case TIMER_FLAG_CALLBACK_IN_IRQ:
			case TIMER_FLAG_CALLBACK_IN_HB:
				if (dev_timer->arg1)
					(*(timer_callback)dev_timer->arg1)(dev_timer->arg2);
				break;
			case TIMER_FLAG_SIGNAL:
				send_sig((int)dev_timer->arg2, (struct task_struct *)dev_timer->arg1, 0);
				break;
			}
		}
	}
	return IRQ_HANDLED;
}

static inline void lq_enable_gptu(void)
{
	struct clk *clk = clk_get_sys("ltq_gptu", NULL);
	clk_enable(clk);

	//ltq_pmu_enable(PMU_GPT);

	/*  Set divider as 1, disable write protection for SPEN, enable module. */
	*LQ_GPTU_CLC =
		GPTU_CLC_SMC_SET(0x00) |
		GPTU_CLC_RMC_SET(0x01) |
		GPTU_CLC_FSOE_SET(0) |
		GPTU_CLC_SBWE_SET(1) |
		GPTU_CLC_EDIS_SET(0) |
		GPTU_CLC_SPEN_SET(0) |
		GPTU_CLC_DISR_SET(0);
}

static inline void lq_disable_gptu(void)
{
	struct clk *clk = clk_get_sys("ltq_gptu", NULL);
	ltq_w32(0x00, LQ_GPTU_IRNEN);
	ltq_w32(0xfff, LQ_GPTU_IRNCR);

	/*  Set divider as 0, enable write protection for SPEN, disable module. */
	*LQ_GPTU_CLC =
		GPTU_CLC_SMC_SET(0x00) |
		GPTU_CLC_RMC_SET(0x00) |
		GPTU_CLC_FSOE_SET(0) |
		GPTU_CLC_SBWE_SET(0) |
		GPTU_CLC_EDIS_SET(0) |
		GPTU_CLC_SPEN_SET(0) |
		GPTU_CLC_DISR_SET(1);

	clk_enable(clk);
}

int lq_request_timer(unsigned int timer, unsigned int flag,
	unsigned long value, unsigned long arg1, unsigned long arg2)
{
	int ret = 0;
	unsigned int con_reg, irnen_reg;
	int n, X;

	if (timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	printk(KERN_INFO "request_timer(%d, 0x%08X, %lu)...",
		timer, flag, value);

	if (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT)
		value &= 0xFFFF;
	else
		timer &= ~0x01;

	mutex_lock(&timer_dev.gptu_mutex);

	/*
	 *  Allocate timer.
	 */
	if (timer < FIRST_TIMER) {
		unsigned int mask;
		unsigned int shift;
		/* This takes care of TIMER1B which is the only choice for Voice TAPI system */
		unsigned int offset = TIMER2A;

		/*
		 *  Pick up a free timer.
		 */
		if (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT) {
			mask = 1 << offset;
			shift = 1;
		} else {
			mask = 3 << offset;
			shift = 2;
		}
		for (timer = offset;
		     timer < offset + timer_dev.number_of_timers;
		     timer += shift, mask <<= shift)
			if (!(timer_dev.occupation & mask)) {
				timer_dev.occupation |= mask;
				break;
			}
		if (timer >= offset + timer_dev.number_of_timers) {
			printk("failed![%d]\n", __LINE__);
			mutex_unlock(&timer_dev.gptu_mutex);
			return -EINVAL;
		} else
			ret = timer;
	} else {
		register unsigned int mask;

		/*
		 *  Check if the requested timer is free.
		 */
		mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
		if ((timer_dev.occupation & mask)) {
			printk("failed![%d] mask %#x, timer_dev.occupation %#x\n",
				__LINE__, mask, timer_dev.occupation);
			mutex_unlock(&timer_dev.gptu_mutex);
			return -EBUSY;
		} else {
			timer_dev.occupation |= mask;
			ret = 0;
		}
	}

	/*
	 *  Prepare control register value.
	 */
	switch (TIMER_FLAG_MASK_EDGE(flag)) {
	default:
	case TIMER_FLAG_NONE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x00);
		break;
	case TIMER_FLAG_RISE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x01);
		break;
	case TIMER_FLAG_FALL_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x02);
		break;
	case TIMER_FLAG_ANY_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x03);
		break;
	}
	if (TIMER_FLAG_MASK_TYPE(flag) == TIMER_FLAG_TIMER)
		con_reg |=
			TIMER_FLAG_MASK_SRC(flag) ==
			TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EXT_SET(1) :
			GPTU_CON_SRC_EXT_SET(0);
	else
		con_reg |=
			TIMER_FLAG_MASK_SRC(flag) ==
			TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EG_SET(1) :
			GPTU_CON_SRC_EG_SET(0);
	con_reg |=
		TIMER_FLAG_MASK_SYNC(flag) ==
		TIMER_FLAG_UNSYNC ? GPTU_CON_SYNC_SET(0) :
		GPTU_CON_SYNC_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_INVERT(flag) ==
		TIMER_FLAG_REAL ? GPTU_CON_INV_SET(0) : GPTU_CON_INV_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_SIZE(flag) ==
		TIMER_FLAG_16BIT ? GPTU_CON_EXT_SET(0) :
		GPTU_CON_EXT_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_STOP(flag) ==
		TIMER_FLAG_ONCE ? GPTU_CON_STP_SET(1) : GPTU_CON_STP_SET(0);
	con_reg |=
		TIMER_FLAG_MASK_TYPE(flag) ==
		TIMER_FLAG_TIMER ? GPTU_CON_CNT_SET(0) :
		GPTU_CON_CNT_SET(1);
	con_reg |=
		TIMER_FLAG_MASK_DIR(flag) ==
		TIMER_FLAG_UP ? GPTU_CON_DIR_SET(1) : GPTU_CON_DIR_SET(0);

	/*
	 *  Fill up running data.
	 */
	timer_dev.timer[timer - FIRST_TIMER].flag = flag;
	timer_dev.timer[timer - FIRST_TIMER].arg1 = arg1;
	timer_dev.timer[timer - FIRST_TIMER].arg2 = arg2;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer_dev.timer[timer - FIRST_TIMER + 1].flag = flag;

	/*
	 *  Enable GPTU module.
	 */
	if (!timer_dev.f_gptu_on) {
		lq_enable_gptu();
		timer_dev.f_gptu_on = 1;
	}

	/*
	 *  Enable IRQ.
	 */
	if (TIMER_FLAG_MASK_HANDLE(flag) != TIMER_FLAG_NO_HANDLE) {
		if (TIMER_FLAG_MASK_HANDLE(flag) == TIMER_FLAG_SIGNAL)
			timer_dev.timer[timer - FIRST_TIMER].arg1 =
				(unsigned long) find_task_by_vpid((int) arg1);

		irnen_reg = 1 << (timer - FIRST_TIMER);

		if (TIMER_FLAG_MASK_HANDLE(flag) == TIMER_FLAG_SIGNAL
		    || (TIMER_FLAG_MASK_HANDLE(flag) ==
			TIMER_FLAG_CALLBACK_IN_IRQ
			&& timer_dev.timer[timer - FIRST_TIMER].arg1)) {
			enable_irq(timer_dev.timer[timer - FIRST_TIMER].irq);
			timer_dev.timer[timer - FIRST_TIMER].f_irq_on = 1;
		}
	} else
		irnen_reg = 0;

	/*
	 *  Write config register, reload value and enable interrupt.
	 */
	n = timer >> 1;
	X = timer & 0x01;
	*LQ_GPTU_CON(n, X) = con_reg;
	*LQ_GPTU_RELOAD(n, X) = value;
	/* printk("reload value = %d\n", (u32)value); */
	*LQ_GPTU_IRNEN |= irnen_reg;

	mutex_unlock(&timer_dev.gptu_mutex);
	printk("successful!\n");
	return ret;
}
EXPORT_SYMBOL(lq_request_timer);

int lq_free_timer(unsigned int timer)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	if (GPTU_CON_EN(n, X))
		*LQ_GPTU_RUN(n, X) = GPTU_RUN_CEN_SET(1);

	*LQ_GPTU_IRNEN &= ~GPTU_IRNEN_TC_SET(n, X, 1);
	*LQ_GPTU_IRNCR |= GPTU_IRNCR_TC_SET(n, X, 1);

	if (timer_dev.timer[timer - FIRST_TIMER].f_irq_on) {
		disable_irq(timer_dev.timer[timer - FIRST_TIMER].irq);
		timer_dev.timer[timer - FIRST_TIMER].f_irq_on = 0;
	}

	timer_dev.occupation &= ~mask;
	if (!timer_dev.occupation && timer_dev.f_gptu_on) {
		lq_disable_gptu();
		timer_dev.f_gptu_on = 0;
	}

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(lq_free_timer);

int lq_start_timer(unsigned int timer, int is_resume)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) ==
	TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*LQ_GPTU_RUN(n, X) = GPTU_RUN_RL_SET(!is_resume) | GPTU_RUN_SEN_SET(1);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(lq_start_timer);

int lq_stop_timer(unsigned int timer)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER
	    || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*LQ_GPTU_RUN(n, X) = GPTU_RUN_CEN_SET(1);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(lq_stop_timer);

int lq_reset_counter_flags(u32 timer, u32 flags)
{
	unsigned int oflag;
	unsigned int mask, con_reg;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	oflag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(oflag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(oflag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	switch (TIMER_FLAG_MASK_EDGE(flags)) {
	default:
	case TIMER_FLAG_NONE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x00);
		break;
	case TIMER_FLAG_RISE_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x01);
		break;
	case TIMER_FLAG_FALL_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x02);
		break;
	case TIMER_FLAG_ANY_EDGE:
		con_reg = GPTU_CON_EDGE_SET(0x03);
		break;
	}
	if (TIMER_FLAG_MASK_TYPE(flags) == TIMER_FLAG_TIMER)
		con_reg |= TIMER_FLAG_MASK_SRC(flags) == TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EXT_SET(1) : GPTU_CON_SRC_EXT_SET(0);
	else
		con_reg |= TIMER_FLAG_MASK_SRC(flags) == TIMER_FLAG_EXT_SRC ? GPTU_CON_SRC_EG_SET(1) : GPTU_CON_SRC_EG_SET(0);
	con_reg |= TIMER_FLAG_MASK_SYNC(flags) == TIMER_FLAG_UNSYNC ? GPTU_CON_SYNC_SET(0) : GPTU_CON_SYNC_SET(1);
	con_reg |= TIMER_FLAG_MASK_INVERT(flags) == TIMER_FLAG_REAL ? GPTU_CON_INV_SET(0) : GPTU_CON_INV_SET(1);
	con_reg |= TIMER_FLAG_MASK_SIZE(flags) == TIMER_FLAG_16BIT ? GPTU_CON_EXT_SET(0) : GPTU_CON_EXT_SET(1);
	con_reg |= TIMER_FLAG_MASK_STOP(flags) == TIMER_FLAG_ONCE ? GPTU_CON_STP_SET(1) : GPTU_CON_STP_SET(0);
	con_reg |= TIMER_FLAG_MASK_TYPE(flags) == TIMER_FLAG_TIMER ? GPTU_CON_CNT_SET(0) : GPTU_CON_CNT_SET(1);
	con_reg |= TIMER_FLAG_MASK_DIR(flags) == TIMER_FLAG_UP ? GPTU_CON_DIR_SET(1) : GPTU_CON_DIR_SET(0);

	timer_dev.timer[timer - FIRST_TIMER].flag = flags;
	if (TIMER_FLAG_MASK_SIZE(flags) != TIMER_FLAG_16BIT)
		timer_dev.timer[timer - FIRST_TIMER + 1].flag = flags;

	n = timer >> 1;
	X = timer & 0x01;

	*LQ_GPTU_CON(n, X) = con_reg;
	smp_wmb();
	printk(KERN_INFO "[%s]: counter%d oflags %#x, nflags %#x, GPTU_CON %#x\n", __func__, timer, oflag, flags, *LQ_GPTU_CON(n, X));
	mutex_unlock(&timer_dev.gptu_mutex);
	return 0;
}
EXPORT_SYMBOL(lq_reset_counter_flags);

int lq_get_count_value(unsigned int timer, unsigned long *value)
{
	unsigned int flag;
	unsigned int mask;
	int n, X;

	if (!timer_dev.f_gptu_on)
		return -EINVAL;

	if (timer < FIRST_TIMER
	    || timer >= FIRST_TIMER + timer_dev.number_of_timers)
		return -EINVAL;

	mutex_lock(&timer_dev.gptu_mutex);

	flag = timer_dev.timer[timer - FIRST_TIMER].flag;
	if (TIMER_FLAG_MASK_SIZE(flag) != TIMER_FLAG_16BIT)
		timer &= ~0x01;

	mask = (TIMER_FLAG_MASK_SIZE(flag) == TIMER_FLAG_16BIT ? 1 : 3) << timer;
	if (((timer_dev.occupation & mask) ^ mask)) {
		mutex_unlock(&timer_dev.gptu_mutex);
		return -EINVAL;
	}

	n = timer >> 1;
	X = timer & 0x01;

	*value = *LQ_GPTU_COUNT(n, X);

	mutex_unlock(&timer_dev.gptu_mutex);

	return 0;
}
EXPORT_SYMBOL(lq_get_count_value);

u32 lq_cal_divider(unsigned long freq)
{
	u64 module_freq, fpi = ltq_get_fpi_bus_clock(2);
	u32 clock_divider = 1;
	module_freq = fpi * 1000;
	do_div(module_freq, clock_divider * freq);
	return module_freq;
}
EXPORT_SYMBOL(lq_cal_divider);

int lq_set_timer(unsigned int timer, unsigned int freq, int is_cyclic,
	int is_ext_src, unsigned int handle_flag, unsigned long arg1,
	unsigned long arg2)
{
	unsigned long divider;
	unsigned int flag;

	divider = lq_cal_divider(freq);
	if (divider == 0)
		return -EINVAL;
	flag = ((divider & ~0xFFFF) ? TIMER_FLAG_32BIT : TIMER_FLAG_16BIT)
		| (is_cyclic ? TIMER_FLAG_CYCLIC : TIMER_FLAG_ONCE)
		| (is_ext_src ? TIMER_FLAG_EXT_SRC : TIMER_FLAG_INT_SRC)
		| TIMER_FLAG_TIMER | TIMER_FLAG_DOWN
		| TIMER_FLAG_MASK_HANDLE(handle_flag);

	printk(KERN_INFO "lq_set_timer(%d, %d), divider = %lu\n",
		timer, freq, divider);
	return lq_request_timer(timer, flag, divider, arg1, arg2);
}
EXPORT_SYMBOL(lq_set_timer);

int lq_set_counter(unsigned int timer, unsigned int flag, u32 reload,
	unsigned long arg1, unsigned long arg2)
{
	printk(KERN_INFO "lq_set_counter(%d, %#x, %d)\n", timer, flag, reload);
	return lq_request_timer(timer, flag, reload, arg1, arg2);
}
EXPORT_SYMBOL(lq_set_counter);

static long gptu_ioctl(struct file *file, unsigned int cmd,
	unsigned long arg)
{
	int ret;
	struct gptu_ioctl_param param;

	if (!access_ok(VERIFY_READ, arg, sizeof(struct gptu_ioctl_param)))
		return -EFAULT;
	copy_from_user(&param, (void *) arg, sizeof(param));

	if ((((cmd == GPTU_REQUEST_TIMER || cmd == GPTU_SET_TIMER
	       || GPTU_SET_COUNTER) && param.timer < 2)
	     || cmd == GPTU_GET_COUNT_VALUE || cmd == GPTU_CALCULATE_DIVIDER)
	    && !access_ok(VERIFY_WRITE, arg,
			   sizeof(struct gptu_ioctl_param)))
		return -EFAULT;

	switch (cmd) {
	case GPTU_REQUEST_TIMER:
		ret = lq_request_timer(param.timer, param.flag, param.value,
				     (unsigned long) param.pid,
				     (unsigned long) param.sig);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
	case GPTU_FREE_TIMER:
		ret = lq_free_timer(param.timer);
		break;
	case GPTU_START_TIMER:
		ret = lq_start_timer(param.timer, param.flag);
		break;
	case GPTU_STOP_TIMER:
		ret = lq_stop_timer(param.timer);
		break;
	case GPTU_GET_COUNT_VALUE:
		ret = lq_get_count_value(param.timer, &param.value);
		if (!ret)
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      value, &param.value,
				      sizeof(param.value));
		break;
	case GPTU_CALCULATE_DIVIDER:
		param.value = lq_cal_divider(param.value);
		if (param.value == 0)
			ret = -EINVAL;
		else {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      value, &param.value,
				      sizeof(param.value));
			ret = 0;
		}
		break;
	case GPTU_SET_TIMER:
		ret = lq_set_timer(param.timer, param.value,
				 TIMER_FLAG_MASK_STOP(param.flag) !=
				 TIMER_FLAG_ONCE ? 1 : 0,
				 TIMER_FLAG_MASK_SRC(param.flag) ==
				 TIMER_FLAG_EXT_SRC ? 1 : 0,
				 TIMER_FLAG_MASK_HANDLE(param.flag) ==
				 TIMER_FLAG_SIGNAL ? TIMER_FLAG_SIGNAL :
				 TIMER_FLAG_NO_HANDLE,
				 (unsigned long) param.pid,
				 (unsigned long) param.sig);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
	case GPTU_SET_COUNTER:
		lq_set_counter(param.timer, param.flag, param.value, 0, 0);
		if (ret > 0) {
			copy_to_user(&((struct gptu_ioctl_param *) arg)->
				      timer, &ret, sizeof(&ret));
			ret = 0;
		}
		break;
	default:
		ret = -ENOTTY;
	}

	return ret;
}

static int gptu_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int gptu_release(struct inode *inode, struct file *file)
{
	return 0;
}

int __init lq_gptu_init(void)
{
	int ret;
	unsigned int i;

	ltq_w32(0, LQ_GPTU_IRNEN);
	ltq_w32(0xfff, LQ_GPTU_IRNCR);

	memset(&timer_dev, 0, sizeof(timer_dev));
	mutex_init(&timer_dev.gptu_mutex);

	lq_enable_gptu();
	timer_dev.number_of_timers = GPTU_ID_CFG * 2;
	lq_disable_gptu();
	if (timer_dev.number_of_timers > MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2)
		timer_dev.number_of_timers = MAX_NUM_OF_32BIT_TIMER_BLOCKS * 2;
	printk(KERN_INFO "gptu: totally %d 16-bit timers/counters\n", timer_dev.number_of_timers);

	ret = misc_register(&gptu_miscdev);
	if (ret) {
		printk(KERN_ERR "gptu: can't misc_register, get error %d\n", -ret);
		return ret;
	} else {
		printk(KERN_INFO "gptu: misc_register on minor %d\n", gptu_miscdev.minor);
	}

	for (i = 0; i < timer_dev.number_of_timers; i++) {
		ret = request_irq(TIMER_INTERRUPT + i, timer_irq_handler, IRQF_TIMER, gptu_miscdev.name, &timer_dev.timer[i]);
		if (ret) {
			for (; i >= 0; i--)
				free_irq(TIMER_INTERRUPT + i, &timer_dev.timer[i]);
			misc_deregister(&gptu_miscdev);
			printk(KERN_ERR "gptu: failed in requesting irq (%d), get error %d\n", i, -ret);
			return ret;
		} else {
			timer_dev.timer[i].irq = TIMER_INTERRUPT + i;
			disable_irq(timer_dev.timer[i].irq);
			printk(KERN_INFO "gptu: succeeded to request irq %d\n", timer_dev.timer[i].irq);
		}
	}

	return 0;
}

void __exit lq_gptu_exit(void)
{
	unsigned int i;

	for (i = 0; i < timer_dev.number_of_timers; i++) {
		if (timer_dev.timer[i].f_irq_on)
			disable_irq(timer_dev.timer[i].irq);
		free_irq(timer_dev.timer[i].irq, &timer_dev.timer[i]);
	}
	lq_disable_gptu();
	misc_deregister(&gptu_miscdev);
}

module_init(lq_gptu_init);
module_exit(lq_gptu_exit);
