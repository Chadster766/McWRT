#!/bin/sh
N="
"

SW_DEVICE
SW_DRIVER="Unknow"
SW_PORT_CPU=0
SW_PORT_BASE=0
SW_PORT_MAX=0
SW_PORT_COUNT=0
SW_VLAN_BASE=0
SW_VLAN_MAX=0
SW_VLAN_COUNT=0

switch_info() {
	[ -z "$1" ] && {
		echo "Usage: $0 <dev>"
		exit 0
	}
	SW_DEVICE=$1
	[ -e /proc/switch ] && {
		[ -e /proc/switch/$SW_DEVICE/driver ] && {
			SW_DRIVER=$(cat /proc/switch/$SW_DEVICE/driver)
			SW_PORT_CPU=5
			for value in $(ls /proc/switch/$SW_DEVICE/vlan); do
				let "SW_VLAN_COUNT+=1"
				if [ $value -gt $SW_VLAN_MAX ]; then
					SW_VLAN_MAX=$value
				fi
				if [ $value -lt $SW_VLAN_BASE ]; then
					SW_VLAN_BASE=$value
				fi
			done
			for value in $(ls /proc/switch/$SW_DEVICE/port/); do
				let "SW_PORT_COUNT+=1"
				if [ $value -gt $SW_PORT_MAX ]; then
					SW_PORT_MAX=$value
				fi
				if [ $value -lt $SW_PORT_BASE ]; then
					SW_PORT_BASE=$value
				fi
			done
			# bcm board assume cpu is 5
			SW_PORT_CPU=5
#			echo "Device: $SW_DEVICE${N}Driver: $SW_DRIVER${N}Ports: $SW_PORT_COUNT${N}PortCPU: $SW_PORT_CPU${N}Vlans: $SW_VLAN_COUNT"
#		} || {
#			echo "Failed to connect to the switch"
		}
	} || {
		strswitch=$(swconfig dev $SW_DEVICE help | head -n1) 
		[ "$?" != "0" ] && {
			for value in $(echo $strswitch | sed 's/Switch[ 0-9]*/Driver/' | sed 's/ @ /=/' | sed 's/: /=/g' | awk -F", " '{ 
				print substr($1,1,index($1,"(")-1)"\n"substr($2,0,index($2," ("))"\n"substr($2,index($2," (")+2,length($2)-index($2," (")-2)"\n"$3"\n"$4}'); do
				eval $value
			done
			SW_DRIVER=$Driver
			SW_PORT_COUNT=$ports
			SW_PORT_CPU=$cpu
			SW_PORT_BASE=0
			let "SW_PORT_MAX=$ports-1"
			SW_VLAN_COUNT=$vlans
			let "SW_VLAN_MAX=$vlans-1"
			SW_VLAN_BASE=0
#			echo "Device: $SW_DEVICE${N}Driver: $SW_DRIVER${N}Ports: $SW_PORT_COUNT${N}PortCPU: $SW_PORT_CPU${N}Vlans: $SW_VLAN_COUNT"
#		} || {
#			echo $strswitch
		}
	}
}