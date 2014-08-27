DRIVER-OBJS = ap8x-objs

driver-objs := \
		driver/linux/ap8xLnxDesc.o 	\
		driver/linux/ap8xLnxIntf.o		\
		driver/linux/ap8xLnxApi.o		\
		driver/linux/ap8xLnxRecv.o		\
		driver/linux/ap8xLnxXmit.o		\
		driver/linux/ap8xLnxStat.o		\
		driver/linux/ap8xLnxWlLog.o	\
		driver/linux/ap8xLnxDump.o \
		core/os/os_if.o \
		core/os/timer.o
		
driver-lib := core/lib_w8864.a

$(DRIVER-OBJS)	:= $(driver-objs) $(driver-lib)
		
obj-m += ap8x.o

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc

EXTRA_CFLAGS+= -I${KDIR}
EXTRA_CFLAGS+= -O2 -funroll-loops

EXTRA_CFLAGS+=-I${PWD} -I${PWD} -I${PWD}/driver/linux -I${PWD}/core/api -I${PWD}/core/incl -I${PWD}/core/mgt/STA/incl

EXTRA_CFLAGS+=-DMV_CPU_LE -DNUMOFAPS=8

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *.o *.a *.s *.ko *.ko.cmd *.o.cmd *.mod.* .ap8x.* $(driver-objs)
	find . -name .*.o.cmd -exec rm -f {} \;
