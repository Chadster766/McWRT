#!/bin/sh

. /etc/functions-net.sh

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

