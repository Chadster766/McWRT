. /usr/share/libubox/jshn.sh

__network_set_cache()
{
	if [ -n "$3" ]; then
		eval "export -- __NETWORK_CV_$1='$3'"
		__NETWORK_CACHE="${__NETWORK_CACHE:+$__NETWORK_CACHE }__NETWORK_CV_$1"
	elif json_get_var "__NETWORK_CV_$1" "$2"; then
		__NETWORK_CACHE="${__NETWORK_CACHE:+$__NETWORK_CACHE }__NETWORK_CV_$1"
	fi
}

__network_export()
{
	local __v="__NETWORK_CV_$2"
	eval "export -- \"$1=\${$__v:+\${$__v$4}$3}\"; [ -n \"\${$__v+x}\" ]"
}

__network_parse_ifstatus()
{
	local __iface="$1"
	local __key="${__iface}"
	local __tmp
	local __idx
	local __list
	local __old_ns

	case "$__iface" in
		*[^a-zA-Z0-9_]*) return 1 ;;
	esac

	__network_export __tmp "${__key}__parsed" && return 0
	__tmp="$(ubus call network.interface."$__iface" status 2>/dev/null)"
	[ -n "$__tmp" ] || return 1

	json_set_namespace "network" __old_ns
	json_load "$__tmp"

	__network_set_cache "${__key}__parsed" "" "1"

	for __tmp in "" "_inactive"; do

		__key="${__key}${__tmp}"

		# parse addresses
		local __family
		for __family in 4 6; do

			__list=""

			if json_is_a "ipv${__family}_address" array; then

				json_select "ipv${__family}_address"

				__idx=1

				while json_is_a "$__idx" object; do

					json_select "$((__idx++))"
					json_get_var __tmp "address" && __list="${__list:+$__list }$__tmp"
					json_get_var __tmp "mask"    && __list="${__list:+$__list/}$__tmp"
					json_select ".."

				done

				json_select ".."

			fi

			if json_is_a "ipv${__family}_prefix_assignment" array; then

				json_select "ipv${__family}_prefix_assignment"

				__idx=1

				while json_is_a "$__idx" object; do

					json_select "$((__idx++))"
					json_get_var __tmp "address" && __list="${__list:+$__list }${__tmp}1"
					json_get_var __tmp "mask"    && __list="${__list:+$__list/}$__tmp"
					json_select ".."

				done

				json_select ".."

			fi

			if [ -n "$__list" ]; then
				__network_set_cache "${__key}_address${__family}" "" "$__list"
			fi

		done

		# parse prefixes
		if json_is_a "ipv6_prefix" array; then
			json_select "ipv6_prefix"

			__idx=1
			__list=""

			while json_is_a "$__idx" object; do

				json_select "$((__idx++))"
				json_get_var __tmp "address" && __list="${__list:+$__list }$__tmp"
				json_get_var __tmp "mask"    && __list="${__list:+$__list/}$__tmp"
				json_select ".."

			done

			json_select ".."


			if [ -n "$__list" ]; then
				__network_set_cache "${__key}_prefix6" "" "$__list"
			fi

		fi

		# parse routes
		if json_is_a route array; then

			json_select "route"

			local __idx=1
			while json_is_a "$__idx" object; do

				json_select "$((__idx++))"
				json_get_var __tmp table

				if [ -z "$__tmp" ]; then
					json_get_var __tmp target

					case "${__tmp}" in
						0.0.0.0)
							__network_set_cache "${__key}_gateway4" nexthop
						;;
						::)
							__network_set_cache "${__key}_gateway6" nexthop
						;;
					esac
				fi

				json_select ".."

			done

			json_select ".."

		fi

		# parse dns info
		local __field
		for __field in "dns_server" "dns_search"; do
			if json_is_a "$__field" array; then

				json_select "$__field"

				__idx=1
				__list=""

				while json_is_a "$__idx" string; do

					json_get_var __tmp "$((__idx++))"
					__list="${__list:+$__list }$__tmp"

				done

				json_select ".."

				if [ -n "$__list" ]; then
					__network_set_cache "${__key}_${__field}" "" "$__list"
				fi
			fi
		done

		# parse up state, device and physdev
		for __field in "up" "l3_device" "device"; do
			if json_get_type __tmp "$__field"; then
				__network_set_cache "${__key}_${__field}" "$__field"
			fi
		done

		# descend into inactive table
		json_is_a "inactive" object && json_select "inactive"

	done

	json_cleanup
	json_set_namespace "$__old_ns"

	return 0
}


__network_ipaddr()
{
	local __var="$1"
	local __iface="$2"
	local __field="$3"
	local __subst="$4"
	local __list="$5"
	local __tmp=""

	__network_parse_ifstatus "$__iface" || return 1

	if [ $__list = 1 ] && [ -n "$__subst" ]; then
		__network_export "__list" "${__iface}_${__field}"

		for __list in $__list; do
			eval "__tmp=\"${__tmp:+$__tmp }\${__list$__subst}\""
		done

		export -- "$__var=$__tmp"; [ -n "$__tmp" ]
		return $?
	fi

	__network_export "$__var" "${__iface}_${__field}" "" "$__subst"
	return $?
}

# determine first IPv4 address of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddr()  { __network_ipaddr "$1" "$2" "address4" "%%/*" 0; }

# determine first IPv6 address of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddr6() { __network_ipaddr "$1" "$2" "address6" "%%/*" 0; }

# determine first IPv4 subnet of given logical interface
# 1: destination variable
# 2: interface
network_get_subnet()  { __network_ipaddr "$1" "$2" "address4" "%% *" 0; }

# determine first IPv6 subnet of given logical interface
# 1: destination variable
# 2: interface
network_get_subnet6() { __network_ipaddr "$1" "$2" "address6" "%% *" 0; }

# determine first IPv6 prefix of given logical interface
# 1: destination variable
# 2: interface
network_get_prefix6() { __network_ipaddr "$1" "$2" "prefix6" "%% *" 0; }

# determine all IPv4 addresses of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddrs()  { __network_ipaddr "$1" "$2" "address4" "%%/*" 1; }

# determine all IPv6 addresses of given logical interface
# 1: destination variable
# 2: interface
network_get_ipaddrs6() { __network_ipaddr "$1" "$2" "address6" "%%/*" 1; }

# determine all IPv4 subnets of given logical interface
# 1: destination variable
# 2: interface
network_get_subnets()  { __network_ipaddr "$1" "$2" "address4" "" 1; }

# determine all IPv6 subnets of given logical interface
# 1: destination variable
# 2: interface
network_get_subnets6() { __network_ipaddr "$1" "$2" "address6" "" 1; }

# determine all IPv6 prefixes of given logical interface
# 1: destination variable
# 2: interface
network_get_prefixes6() { __network_ipaddr "$1" "$2" "prefix6" "" 1; }


__network_gateway()
{
	local __var="$1"
	local __iface="$2"
	local __family="$3"
	local __inactive="$4"

	__network_parse_ifstatus "$__iface" || return 1

	if [ "$__inactive" = 1 -o "$__inactive" = "true" ]; then
		__network_export "$__var" "${__iface}_inactive_gateway${__family}" && \
			return 0
	fi

	__network_export "$__var" "${__iface}_gateway${__family}"
	return $?
}

# determine IPv4 gateway of given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive gateway if "true" (optional)
network_get_gateway()  { __network_gateway "$1" "$2" 4 "${3:-0}"; }

# determine  IPv6 gateway of given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive gateway if "true" (optional)
network_get_gateway6() { __network_gateway "$1" "$2" 6 "${3:-0}"; }


__network_dns() {
	local __var="$1"
	local __iface="$2"
	local __field="$3"
	local __inactive="$4"

	__network_parse_ifstatus "$__iface" || return 1

	if [ "$__inactive" = 1 -o "$__inactive" = "true" ]; then
		__network_export "$__var" "${__iface}_inactive_${__field}" && \
			return 0
	fi

	__network_export "$__var" "${__iface}_${__field}"
	return $?
}

# determine the DNS servers of the given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive servers if "true" (optional)
network_get_dnsserver() { __network_dns "$1" "$2" dns_server "${3:-0}"; }

# determine the domains of the given logical interface
# 1: destination variable
# 2: interface
# 3: consider inactive domains if "true" (optional)
network_get_dnssearch() { __network_dns "$1" "$2" dns_search "${3:-0}"; }


__network_wan()
{
	local __var="$1"
	local __family="$2"
	local __inactive="$3"
	local __iface

	for __iface in $(ubus list | sed -ne 's/^network\.interface\.//p'); do
		if [ "$__iface" != loopback ]; then
			if __network_gateway "$__var" "$__iface" "$__family" "$__inactive"; then
				eval "export -- \"$__var=$__iface\""
				return 0
			fi
		fi
	done

	eval "export -- \"$__var=\""
	return 1
}

# find the logical interface which holds the current IPv4 default route
# 1: destination variable
# 2: consider inactive default routes if "true" (optional)
network_find_wan()  { __network_wan "$1" 4 "${2:-0}"; }

# find the logical interface which holds the current IPv6 default route
# 1: destination variable
# 2: consider inactive dafault routes if "true" (optional)
network_find_wan6() { __network_wan "$1" 6 "${2:-0}"; }


__network_device()
{
	local __var="$1"
	local __iface="$2"
	local __field="$3"

	__network_parse_ifstatus "$__iface" || return 1
	__network_export "$__var" "${__iface}_${__field}"
	return $?
}

# test whether the given logical interface is running
# 1: interface
network_is_up()
{
	local __up
	__network_device __up "$1" up && [ $__up -eq 1 ]
}

# determine the layer 3 linux network device of the given logical interface
# 1: destination variable
# 2: interface
network_get_device()  { __network_device "$1" "$2" l3_device; }

# determine the layer 2 linux network device of the given logical interface
# 1: destination variable
# 2: interface
network_get_physdev() { __network_device "$1" "$2" device;    }


__network_defer()
{
	local __device="$1"
	local __defer="$2"

	json_init
	json_add_string name "$__device"
	json_add_boolean defer "$__defer"

	ubus call network.device set_state "$(json_dump)" 2>/dev/null
}

# defer netifd actions on the given linux network device
# 1: device name
network_defer_device() { __network_defer "$1" 1; }

# continue netifd actions on the given linux network device
# 1: device name
network_ready_device() { __network_defer "$1" 0; }

# flush the internal value cache to force re-reading values from ubus
network_flush_cache()
{
	local __tmp
	for __tmp in $__NETWORK_CACHE __NETWORK_CACHE; do
		unset "$__tmp"
	done
}
