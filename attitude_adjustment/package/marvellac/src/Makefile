DRIVER-OBJS = ap8x-objs

APS=8
#export ARCH=arm
#export STAGING_DIR=/build/henry/mamba/mamba-openwrt-12.09/build_dir/target-arm_v7-a_uClibc-0.9.33.2_eabi/root-armadaxp
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

#export CROSS_COMPILE:=/build/henry/mamba/mamba-openwrt-12.09/build_dir/target-arm_v7-a_uClibc-0.9.33.2_eabi/OpenWrt-Toolchain-armadaxp-for-arm_v7-a-gcc-4.6-linaro_uClibc-0.9.33.2_eabi/toolchain-arm_v7-a_gcc-4.6-linaro_uClibc-0.9.33.2_eabi/bin/arm-openwrt-linux-uclibcgnueabi-
#KDIR:=/build/henry/mamba/mamba-openwrt-12.09/build_dir/linux-armadaxp/linux-3.2.36

#AS		= $(CROSS_COMPILE)as
#LD		= $(CROSS_COMPILE)ld
#CC		= $(CROSS_COMPILE)gcc

#SOC:=$(KDIR)/arch/arm/mach-mv88fxx81

#EXTRA_CFLAGS+= -I${KDIR}
#EXTRA_CFLAGS+= -O2 -funroll-loops

#EXTRA_CFLAGS+=-I${PWD} -I${PWD} -I${PWD}/driver/linux -I${PWD}/core/api -I${PWD}/core/incl -I${PWD}/core/mgt/STA/incl
#EXTRA_CFLAGS+=-I${SOC} -I${SOC}/Common -I${SOC}/Soc/idma -I${SOC}/Soc/ctrlEnv -I${SOC}/mv88f5181 -I${SOC}/osServices -I${SOC}/osServices/linux/ArchARM
#EXTRA_CFLAGS+=-I${SOC}/Board/boardEnv -I${SOC}/Board/boardEnv/DB_88FXX81 -I${SOC}/Soc/ctrlEnv/MV_88FXX81 -I${SOC}/osServices/linux

ifeq (BE,$(ENDIAN))
EXTRA_CFLAGS+=-DMV_CPU_BE
else
EXTRA_CFLAGS+=-DMV_CPU_LE
endif

ifeq (16, $(APS))
EXTRA_CFLAGS+= -DMCAST_PS_OFFLOAD_SUPPORT
endif

ifeq (1, $(WLDBG))
EXTRA_CFLAGS+= -DWL_DEBUG
endif

#ifeq (, $(APS))
#EXTRA_CFLAGS+= -DNUMOFAPS=1
#else
#EXTRA_CFLAGS+= -DNUMOFAPS=$(APS)
#endif

#all:
#
#	$(MAKE) -C $(KDIR) SUBDIRS=$(SUBDIRS) modules

clean:
	rm -f *.o *.a *.s *.ko *.ko.cmd *.o.cmd *.mod.* .ap8x.* $(driver-objs)
	find . -name .*.o.cmd -exec rm -f {} \;
