#!/bin/sh

NET=/tmp/net
LOG=$NET/log
SRV_USR=$NET/users
SRV_IPS=$NET/srv_ip
CLI_IPS=$NET/cli_ip
PATH=/sbin:/usr/sbin:$PATH
export PATH

[ -d $NET ] || mkdir -p $NET

rm_entry() {
    grep -v "^$1 " $2 > $2.new
    mv $2.new $2
}

clear_dev() {
    for file in `grep -l "^$1 " $NET/*`; do
	rm_entry $1 $file
    done
}

# net_auth_up ppp $DEV $TTY $USER
net_auth_up() {
    echo "authup: $@" >> $LOG
    rm_entry $2 $SRV_USR.$1
    echo "$2 $3 $4 "`date` >> $SRV_USR.$1
}

# net_auth_down ppp $DEV $TTY $USER
net_auth_down() {
    echo "authdown: $@" >> $LOG
    clean_dev $2
}

#                    $DEV $TTY           $GATEWAY      $TUNNELIP      $CLIENTIP  (when pptpd)
# net_ip_up ppp      ppp0 /dev/pts/0     192.168.200.1 192.168.200.10 192.168.10.25
# net_ip_up ppp      ppp0 192.168.200.10 192.168.200.1 pptp                      (when pptp)
# net_ip_up openvpn  tun0 $TUNNELIP      $GATEWAY      openvpn
net_ip_up() {
    echo "ipup: $@" >> $LOG
    if [ "$6" = "" ]; then
	ext=$5
	[ "$1" != "ppp" ] && ext=$1
	rm_entry $2 $CLI_IPS.$ext
	echo "$2 $3 $4" >> $CLI_IPS.$ext
    else
	rm_entry $2 $SRV_IPS.pptp
	echo "$2 $3 $4 $5 $6" >> $SRV_IPS.pptp
    fi
}

# net_ip_down ppp     $TTY 
# net_ip_down openvpn $TUN
net_ip_down() {
    echo "ipdown: $@" >> $LOG
    clear_dev $2
}


ifconfig_info() {
    ifconfig -a | awk -v "opt=$1" '
BEGIN {
  ifaces=""
  ptp=0
}
($0 ~ /^[^ \t]/) { 
  iface=$1
  gsub(/\.[0-9]+$/,"",iface)
}
($3 ~ /^encap:/) { 
  encap=$3
  gsub(/encap:/,"",encap)
  if (encap == "Ethernet") {
    link=$5
  }
  next
}
($2 ~ /^addr:/) { 
  ip=$2
  gsub(/addr:/,"",ip)

  if ($3 ~ /P-t-P:/) {
    ptp=1
    link=substr($3,7)
  }

  mask=$4
  gsub(/Mask:/,"",mask)
}
($1 == "RX" && $5 == "TX") {
  rx_bytes=$2
  rx_desc=$3$4
  tx_bytes=$6
  tx_desc=$7$8

  gsub(/bytes:/,"",rx_bytes)
  gsub(/bytes:/,"",tx_bytes)
}
($0 ~ /^[ \t]*$/) {
  clifile=""
  "grep -l \"^" iface " \" / /tmp/net/???_ip.* 2>/dev/null | tail -n 1" | getline clifile
  if (clifile != "") { 
    "basename " clifile | getline clifile
    gsub(/..._ip./,"",clifile)
  }

  if (encap != "" && encap != "Local" && (encap != "UNSPEC" || ptp == 1)) {
    if (link == "") { link="-"; }
    if (ip == "") { ip="-"; }
    if (mask == "") { mask="-"; }
    if (clifile == "") { clifile="-"; }
    if (encap == "UNSPEC") { encap="Pnt-to-Pnt"; }

    if (opt == "raw") {
      print iface " " ip " " mask " " encap " " link " " clifile " " rx_bytes " " rx_desc " " tx_bytes " " tx_desc 
    } else {
      print iface "=\"" iface " " ip " " mask " " encap " " link " " clifile " " rx_bytes " " rx_desc " " tx_bytes " " tx_desc "\""
    }

    ifaces=ifaces " " iface
  }
  link=""
  encap=""
  ip=""
  mask=""
  ptp=0
}
END {
  print "ifaces=\"" substr(ifaces,2) "\""
}
'
}

brctrl_ifaces() {
    brctl show | awk '
BEGIN {
  ifaces=""
}
(NF == 4) { 
  iface=$4
  gsub(/\.[0-9]+$/,"",iface)
  ifaces=ifaces " " iface
}
(NF == 1) { 
  iface=$1
  gsub(/\.[0-9]+$/,"",iface)
  ifaces=ifaces " " iface
}
END {
  print "br_ifaces=\"" substr(ifaces,2) "\""
}
'
}

ip2int() {
  set $(echo $1 | tr '\.' ' ');
  echo $(($1<<24|$2<<16|$3<<8|$4));
}

int2ip() {
  echo $(($1>>24&255)).$(($1>>16&255)).$(($1>>8&255)).$(($1&255))
}


#pptpd 
 USERS=/etc/ppp/users
 PEERS=/etc/ppp/peers
 
 # users.pptpd format:
 # username password ip-address
 
 # users.pptp format:
 # peername username password ip-address
 
 # peers.pptp format:
 # peername host-name username 
 
 ppp_del_user() {
     rm_entry "$2" $USERS.$1
 }
 
 ppp_add_user() {
     ppp_del_user "$1" "$2"
     echo "$2 $3 $4 $5" >> $USERS.$1
 }
 
 ppp_del_peer() {
     rm_entry "$2" $USERS.$1
     rm_entry "$2" $PEERS.$1
 }
 
 ppp_add_peer() {
     ppp_add_user "$1" "$2" "$4" "$5" "$6"
     rm_entry "$2" $PEERS.$1
     echo "$2 $3 $4" >> $PEERS.$1
 }
 
 build_chap_secrets() {
     mkdir /etc/ppp/peers 2>&-
     touch /etc/ppp/users.pptpd /etc/ppp/users.pptp /etc/ppp/peers.pptp
     grep -v pptp /etc/ppp/chap-secrets > /tmp/chap-secrets
     awk '{print $1 " pptpd " $2 " " $3}' /etc/ppp/users.pptpd >> /tmp/chap-secrets
     awk '{print $2 " pptp:" $1 " " $3 " " $4}' /etc/ppp/users.pptp >> /tmp/chap-secrets
 
     awk '{
            peer="/etc/ppp/peers/pptp:" $1
            print "pty \"pptp " $2 " --nolaunchpppd\"" > peer
            print "mppe required,stateless" >> peer
            print "name " $3 >> peer
            print "remotename pptp:" $1 >> peer
            print "file /etc/ppp/options.pptp" >> peer
            print "ipparam pptp:" $1 >> peer
          }' /etc/ppp/peers.pptp 
 
     rm /etc/ppp/chap-secrets
     mv /tmp/chap-secrets /etc/ppp/chap-secrets
     chmod 700 /etc/ppp/chap-secrets
     chmod 600 /etc/ppp/peers/* 2>&-
 }