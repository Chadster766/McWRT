#!/bin/sh
./scripts/feeds update packages
./scripts/feeds update luci
./scripts/feeds update routing
./scripts/feeds update telephony
./scripts/feeds update management
./scripts/feeds install -a -p packages
./scripts/feeds install -a -p luci
./scripts/feeds install -a -p routing
./scripts/feeds install -a -p telephony
./scripts/feeds install -a -p management

#Apply feed fixes:
cp ./feed-fixes/170-use-supported-pagers.patch ./feeds/routing/quagga/patches/170-use-supported-pagers.patch
