#!/bin/sh
grep \^CONFIG_TARGET_ .config | head -n3 > tmp/.diffconfig.head
grep '^CONFIG_ALL=y' .config >> tmp/.diffconfig.head
grep '^CONFIG_DEVEL=y' .config >> tmp/.diffconfig.head
grep '^CONFIG_TOOLCHAINOPTS=y' .config >> tmp/.diffconfig.head
./scripts/config/conf -D tmp/.diffconfig.head -w tmp/.diffconfig.stage1 Config.in >/dev/null
./scripts/kconfig.pl '>+' tmp/.diffconfig.stage1 .config >> tmp/.diffconfig.head
./scripts/config/conf -D tmp/.diffconfig.head -w tmp/.diffconfig.stage2 Config.in >/dev/null
./scripts/kconfig.pl '>' tmp/.diffconfig.stage2 .config >> tmp/.diffconfig.head
cat tmp/.diffconfig.head
rm -f tmp/.diffconfig tmp/.diffconfig.head
