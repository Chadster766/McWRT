include $(TOPDIR)/rules.mk

PKG_NAME:=m-route
PKG_VERSION:=0.2
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/m-route
  SECTION:=admin
  CATEGORY:=Network
  DEPENDS:=+ip
  TITLE:=Load Balancing scrip for Multi Wan Connections
endef

define Build/Compile
endef

define Package/m-route/install
	$(CP) -a ./files/* $(1)/
	$(INSTALL_BIN) ./files/etc/hotplug.d/iface/* $(1)/etc/hotplug.d/iface/
	$(INSTALL_BIN) ./files/lib/mroute/* $(1)/lib/mroute/
endef

$(eval $(call BuildPackage,m-route))
