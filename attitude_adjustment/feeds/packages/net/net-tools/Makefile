# 
# Copyright (C) 2006 - 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=net-tools
PKG_VERSION:=1.60
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.tazenda.demon.co.uk/phil/net-tools/
PKG_MD5SUM:=888774accab40217dde927e21979c165

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/net-tools-arp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - arp
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-arp/description
  Program arp from net-tools. Manipulate kernel's ARP cache.
  Add, delete an entry, or to dump the entire cache.
endef

define Package/net-tools-dnsdomainname
  SECTION:=net
  CATEGORY:=Network
  DEPENDS += net-tools-hostname
  TITLE:=net-tools - dnsdomainname  
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-dnsdomainname/description
  Program dnsdomainname from net-tools.
  Reports the system's DNS domain name.
endef

define Package/net-tools-domainname
  SECTION:=net
  CATEGORY:=Network
  DEPENDS += net-tools-hostname
  TITLE:=net-tools - domainname  
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-domainname/description
  Program domainname from net-tools.			       
  Reports the system's NIS/YP domain name.
endef

define Package/net-tools-hostname
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - hostname
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-hostname/description
  Program hostname from net-tools.
  Reports or sets the name of the current host system.
endef

define Package/net-tools-ifconfig
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - ifconfig
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-ifconfig/description
  Program ifconfig from net-tools.
  Configure network interfaces.
endef

define Package/net-tools-ipmaddr
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - ipmaddr
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-ipmaddr/description
  Program ipmaddr from net-tools.  Adds,
  deletes and shows an interface's multicast addresses.
endef

define Package/net-tools-iptunnel
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - iptunnel
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-iptunnel/description
  Program iptunnel from net-tools.
  Adds, changes, deletes and shows an interface's tunnels. 
endef

define Package/net-tools-mii-tool
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - mii-tool
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-mii-tool/description
  Program mii-tool from net-tools.  Checks or sets 
  the status of a network interface's Media Independent 
  Interface (MII) unit.
endef

define Package/net-tools-nameif
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - nameif  
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-nameif/description
  Program nameif from net-tools
  Trivial File Transfer Protocol server.
endef

define Package/net-tools-netstat
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - netstat
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-netstat/description
  Program netstat from net-tools. Report network 
  connections, routing tables, and interface statistics.
endef

define Package/net-tools-nisdomainname
  SECTION:=net
  CATEGORY:=Network
  DEPENDS += net-tools-hostname
  TITLE:=net-tools - nisdomainname
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-nisdomainname/description
  Program nisdomainname from net-tools.
  Same as domainname.
endef

define Package/net-tools-plipconfig
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - plipconfig
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-plipconfig/description
  Program plipconfig from net-tools.  Fine tune the 
  PLIP device parameters, to improve its performance.
endef

define Package/net-tools-rarp
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - rarp
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-rarp/description
  Program rarp from net-tools.
  Manipulate the kernel's RARP table.
endef

define Package/net-tools-route
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  TITLE:=net-tools - route
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-route/description
  Program route from net-tools.
  Manipulate the IP routing table.
endef

define Package/net-tools-slattach
  SECTION:=net
  CATEGORY:=Network
  TITLE:=net-tools - slattach
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-slattach/description
  Program slattach from net-tools.  Attaches a 
  network interface to a serial line. This allows 
  to use normal terminal lines for point-to-point 
  links to other computers.
endef

define Package/net-tools-ypdomainname
  SECTION:=net
  CATEGORY:=Network
  DEPENDS += net-tools-hostname
  TITLE:=net-tools - ypdomainname
  URL:=http://www.tazenda.demon.co.uk/phil/net-tools
endef

define Package/net-tools-ypdomainname/description
  Program route from net-tools.
  Same as domainname.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC="$(TARGET_CC)" \
	AR="$(TARGET_CROSS)ar" \
	COPTS="$(TARGET_CFLAGS)" 
endef

define Package/net-tools-arp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/arp $(1)/usr/bin/
endef

define Package/net-tools-dnsdomainname/install
	$(INSTALL_DIR) $(1)/usr/bin
	ln -sf hostname $(1)/usr/bin/dnsdomainname
endef

define Package/net-tools-domainname/install
	$(INSTALL_DIR) $(1)/usr/bin
	ln -sf hostname $(1)/usr/bin/domainname
endef

define Package/net-tools-hostname/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostname $(1)/usr/bin/
endef

define Package/net-tools-ifconfig/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ifconfig $(1)/usr/sbin/
endef

define Package/net-tools-ipmaddr/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipmaddr $(1)/usr/bin/
endef

define Package/net-tools-iptunnel/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iptunnel $(1)/usr/bin/
endef

define Package/net-tools-mii-tool/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mii-tool $(1)/usr/bin/
endef

define Package/net-tools-nameif/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nameif $(1)/usr/bin/
endef

define Package/net-tools-netstat/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/netstat $(1)/usr/bin/
endef

define Package/net-tools-nisdomainname/install
	$(INSTALL_DIR) $(1)/usr/bin
	ln -sf hostname $(1)/usr/bin/nisdomainname
endef

define Package/net-tools-plipconfig/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/plipconfig $(1)/usr/bin/
endef

define Package/net-tools-rarp/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rarp $(1)/usr/bin/
endef

define Package/net-tools-route/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/route $(1)/usr/bin/
endef

define Package/net-tools-slattach/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/slattach $(1)/usr/bin/
endef

define Package/net-tools-ypdomainname/install
	$(INSTALL_DIR) $(1)/usr/bin
	ln -sf hostname $(1)/usr/bin/ypdomainname
endef

$(eval $(call BuildPackage,net-tools-arp))
$(eval $(call BuildPackage,net-tools-hostname))
$(eval $(call BuildPackage,net-tools-dnsdomainname))
$(eval $(call BuildPackage,net-tools-domainname))
$(eval $(call BuildPackage,net-tools-ifconfig))
$(eval $(call BuildPackage,net-tools-ipmaddr))
$(eval $(call BuildPackage,net-tools-iptunnel))
$(eval $(call BuildPackage,net-tools-mii-tool))
$(eval $(call BuildPackage,net-tools-nameif))
$(eval $(call BuildPackage,net-tools-netstat))
$(eval $(call BuildPackage,net-tools-nisdomainname))
$(eval $(call BuildPackage,net-tools-plipconfig))
$(eval $(call BuildPackage,net-tools-rarp))
$(eval $(call BuildPackage,net-tools-route))
$(eval $(call BuildPackage,net-tools-slattach))
$(eval $(call BuildPackage,net-tools-ypdomainname))
