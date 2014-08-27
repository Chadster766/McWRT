#!/bin/sh

case "$1" in
    if-create)
            ;;
    if-up)
            /sbin/ifdown wan
            /sbin/ifup wimax
            ;;
    if-down)
            /sbin/ifdown wimax
            /sbin/ifup wan
            ;;
    if-release)
            ;;
esac

exit 0