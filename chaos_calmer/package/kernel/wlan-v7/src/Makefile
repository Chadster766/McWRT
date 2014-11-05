#WRT1900AC Wifi Driver test package 

DRIVER-OBJS = ap8x-objs
API-OBJS = ap8x-objs
AP-OBJS = ap8x-objs

$(DRIVER-OBJS)	:= \
		driver/linux/ap8xLnxDesc.o 	\
		driver/linux/ap8xLnxFwdl.o		\
		driver/linux/ap8xLnxFwcmd.o		\
		driver/linux/ap8xLnxIntf.o		\
		driver/linux/ap8xLnxApi.o		\
		driver/linux/ap8xLnxRecv.o		\
		driver/linux/ap8xLnxXmit.o		\
		driver/linux/ap8xLnxStat.o		\
		driver/linux/ap8xLnxWlLog.o	\
		driver/linux/ap8xLnxDump.o

$(API-OBJS)	+= \
		core/api/wlFun.o		\
		core/api/wlApi.o

$(AP-OBJS)	+= \
		core/dbg/wldebug.o 		\
		core/encr/aes_key_wrap.o \
		core/encr/hmac_md5.o \
		core/encr/prfapi.o \
		core/encr/pbkdf2.o \
		core/encr/md5.o \
		core/encr/sha1.o \
		core/encr/tkip.o \
		core/encr/AP/ccmpAP.o \
		core/encr/AP/keyMgmt.o \
		core/encr/AP/tkipAP.o \
		core/wlsys/bcngen.o \
		core/wlsys/hal_mib.o \
		core/wlsys/hal_sys.o \
		core/wlsys/w81init.o \
		core/mgt/mhsm.o \
		core/mgt/List.o \
		core/mgt/timer.o \
		core/mgt/AP/AssocSrvAp.o \
		core/mgt/AP/AuthSrvAp.o \
		core/mgt/AP/idList.o \
		core/mgt/AP/macmgmtap.o \
		core/mgt/AP/macMgmtEvt.o \
		core/mgt/AP/macMgmtMlme.o \
		core/mgt/AP/mlmeApi.o \
		core/mgt/AP/mlmeAuth.o \
		core/mgt/AP/smeMain.o \
		core/mgt/AP/StaDb.o \
		core/mgt/AP/SyncSrvAp.o \
		core/mgt/AP/util.o \
		core/mgt/AP/wlMlmeSrv.o \
		core/mib/mib.o \
		core/mib/wl_mib.o \
		core/os/os_if.o \
		core/qos/qos.o \
		core/qos/qos_back.o \
		core/qos/AP/qos_dlp.o \
		core/qos/AP/qos_scheduler.o \
		core/wlsys/domain.o \
		core/wlsys/packet.o \
		core/wlsys/fragment.o \
		core/wlsys/autochannel.o \
		core/mgt/AP/dfsSM.o \
		core/mgt/AP/dfsEv.o \
		core/wds/wds.o

#ifeq (1, $(BUILD_CLIENT))
$(AP-OBJS)	+= \
		core/mgt/STA/AssocSta_sm.o \
		core/mgt/STA/AssocSta_srv.o \
		core/mgt/STA/AuthSta_sm.o \
		core/mgt/STA/AuthSta_srv.o \
		core/mgt/STA/SyncSta_sm.o \
		core/mgt/STA/SyncSta_srv.o \
		core/mgt/STA/macMgmtEvtSta.o \
		core/mgt/STA/smeStateMgrSta.o \
		core/mgt/STA/mlmeApiSta.o \
		core/mgt/STA/wlvmac.o \
		core/mgt/STA/child_srv.o \
		core/mgt/STA/parent_srv.o \
		core/mgt/STA/wlMlmeSrvSta.o \
		core/wlsys/linkmgt.o \
		core/wlsys/ewb_hash.o \
		core/wlsys/ewb_packet.o \
		core/encr/STA/keyMgmtSta.o
#endif

obj-m += ap8x.o 

clean:
	rm -f *.o *.a *.s *.ko *.ko.cmd *.o.cmd *.mod.* .ap8x.* $(ap8x-objs)
	find . -name .*.o.cmd -exec rm -f {} \;
