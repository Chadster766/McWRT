#!/bin/sh
./scripts/feeds update packages
./scripts/feeds update luci
./scripts/feeds update routing
./scripts/feeds update xwrt
#./scripts/feeds update alljoyn
#./scripts/feeds update wl500g
#./scripts/feeds update mediawrt
#./scripts/feeds update addpack
#./scripts/feeds update meshbox
#./scripts/feeds update external
./scripts/feeds install -a -p packages
./scripts/feeds install -a -p luci
./scripts/feeds install -a -p routing
./scripts/feeds install -a -p xwrt
#./scripts/feeds install -a -p alljoyn
#./scripts/feeds install -a -p wl500g
#./scripts/feeds install -a -p mediawrt
#./scripts/feeds install -a -p addpack
#./scripts/feeds install -a -p meshbox
#./scripts/feeds install -a -p external
#./scripts/feeds update fon
#./scripts/feeds install -a -p fon
