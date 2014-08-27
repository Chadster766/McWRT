
include $(TOPDIR)/rules.mk

PKG_NAME:=perl-www
PKG_VERSION:=5.812
PKG_RELEASE:=1
PKG_MD5SUM:=86c6121170efe55fc0d33cdc967561bc

PKG_SOURCE_URL:=http://search.cpan.org/CPAN/authors/id/G/GA/GAAS
PKG_SOURCE:=libwww-perl-$(PKG_VERSION).tar.gz

PKG_BUILD_DIR:=$(BUILD_DIR)/perl/libwww-perl-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include ../perl/perlmod.mk

define Package/perl-www
SUBMENU:=Perl
SECTION:=lang
CATEGORY:=Languages
TITLE:=WWW client/server library for Perl (aka LWP)
URL:=http://search.cpan.org/dist/libwww-perl/
DEPENDS:=perl +perl-html-parser +perl-uri
endef

define Build/Configure
	$(call perlmod/Configure,-n,)
endef

define Build/Compile
	$(call perlmod/Compile,,)
endef

define Package/perl-www/install
	$(call perlmod/Install,$(1),File HTML HTTP LWP LWP.pm Net WWW)
endef


$(eval $(call BuildPackage,perl-www))
