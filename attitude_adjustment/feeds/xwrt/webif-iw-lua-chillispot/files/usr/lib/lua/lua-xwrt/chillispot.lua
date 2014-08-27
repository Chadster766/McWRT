require("lua-xwrt.addon.string")
require("lua-xwrt.addon.uci")
require("lua-xwrt.html.form")
require("lua-xwrt.xwrt.translator")
require("lua-xwrt.html.messages")

local string = string
local io = io
local os = os
local pairs, ipairs = pairs, ipairs
local table = table
local uci = uci
local util = util
local tr = tr
local formClass = formClass
local string = string
local type = type
local print = print
local tostring, tonumber = tostring, tonumber

-- Save upload configuration file
--[[
if type(__FORM.upload_config) == "table" then
    local file
    if uci.get("chillispot","service","config") == "uci" then
	file = io.open("/etc/config/chillispot","w")
    else
	file = io.open("/etc/chilli.conf","w")
    end
    if file then
	file:write(__FORM.upload_config.data)
	file:close()
    end
end
]]
local newMenu = htmlhmenuClass.new("submenu")
newMenu:add(tr("chilli_menu_service#Service"),"chillispot.sh")
newMenu:add(tr("chilli_menu_tun#TUN"),"chillispot.sh?option=tun")
newMenu:add(tr("chilli_menu_radiuis#Radius"),"chillispot.sh?option=radius")
newMenu:add(tr("chilli_menu_proxy#Proxy"),"chillispot.sh?option=proxy")
newMenu:add(tr("chilli_menu_dhcp#DHCP"),"chillispot.sh?option=dhcp")
newMenu:add(tr("chilli_menu_uam#UAM"),"chillispot.sh?option=uam")
newMenu:add(tr("chilli_menu_uam#MAC Authentication"),"chillispot.sh?option=mac")
newMenu:add(tr("chilli_menu_socket#Remote Access"),"chillispot.sh?option=socket")

if uci.get("chillispot","service","config") == "uci" then
	if __MENU[__FORM.cat].len > 1 then
		__MENU[__FORM.cat]["ChilliSpot"] = newMenu
	else
		__MENU[__FORM.cat]= newMenu
	end
end

uci.check_set("chillispot","service","chillispot")
uci.save("chillispot")

module("lua-xwrt.chillispot")

local tiface = {}
function iface(t)
	tiface[t[".name"]] = t.ifname
end

function interfaces()
	local str = ""
	local savedir = uci.get_savedir()
	uci.set_savedir("/var/state")
	local interfaces = uci.foreach("network","interface",iface)
	uci.set_savedir(savedir)
	return tiface
end

function service()
	local forms = {}
	forms[#forms+1] = formClass.new("Config")
	forms[#forms]:Add("service","chillispot.service.enable","/usr/sbin/chilli",tr("chillispot_var_service#Service"),"")
	forms[#forms]["chillispot.service.enable"].options:Add("service","chilli")
	forms[#forms]["chillispot.service.enable"].options:Add("init","chilli")

--	forms[#forms]:Add("select","chillispot.service.enable",uci.get("chillispot","service","enable",0),tr("chilli_var_enable#Service"),"string")
--	forms[#forms]["chillispot.service.enable"].options:Add("0","Disable")
--	forms[#forms]["chillispot.service.enable"].options:Add("1","Enable")
	forms[#forms]:Add("select","chillispot.service.config",uci.get("chillispot","service","config","UCI"),tr("chilli_var_config#Config type"),"string")
	forms[#forms]["chillispot.service.config"].options:Add("uci","UCI")
	forms[#forms]["chillispot.service.config"].options:Add("/etc/chilli.conf","/etc/chilli.conf")
	forms[#forms]:Add_help(tr("chillispot_var_enable#Service"),tr("chilli_help_enable#Enable or disable service."))
	forms[#forms]:Add("file","upload_config","",tr("chillispot_var_uploadconf#Upload Conf"),"")
	if uci.get("chillispot","service","config") ~= "uci" then
		local conf_data, len = util.file_load("/etc/chilli.conf")
		forms[#forms+1] = formClass.new("Configuration File",true)
		forms[#forms]:Add("text_area", "chilli_conf", conf_data, "/etc/chilli.conf", "string", "float:right;width:100%;height:200px;")
	else
		forms[#forms+1] = formClass.new("Service Settings")
		forms[#forms]:Add("text", "chillispot.settings.interval", uci.get("chillispot","settings","interval"),tr("chilli_var_interval#Interval"), "string")
--		forms[#forms]:Add("text", "chillispot.settings.pidfile", uci.get("chillispot","settings","pidfile"),tr("chilli_var_pidfile#PID file"), "string")
	end
	return forms
end

function tun()
	local forms = {}
	forms[#forms+1] = formClass.new("TUN - Settings")
		forms[#forms]:Add("text", "chillispot.settings.net", uci.get("chillispot","settings","net"),tr("chilli_var_net#Chilli subnet"), "string")
		forms[#forms]:Add("text", "chillispot.settings.dynip", uci.get("chillispot","settings","dynip"),tr("chilli_var_dynip#Dynamics IPs"), "string")
		forms[#forms]:Add("text", "chillispot.settings.statip", uci.get("chillispot","settings","statip"),tr("chilli_var_statip#Statics IPs"), "string")
		forms[#forms]:Add("text", "chillispot.settings.dns1", uci.get("chillispot","settings","dns1"),tr("chilli_var_dns1#Primary DNS"), "string")
		forms[#forms]:Add("text", "chillispot.settings.dns2", uci.get("chillispot","settings","dns2"),tr("chilli_var_dns2#Secondary DNS"), "string")
		forms[#forms]:Add("text", "chillispot.settings.domain", uci.get("chillispot","settings","domain"),tr("chilli_var_domain#Domain"), "string")
		forms[#forms]:Add("text", "chillispot.settings.ipup", uci.get("chillispot","settings","ipup"),tr("chilli_var_ipup#IP Up Script"), "string")
		forms[#forms]:Add("text", "chillispot.settings.ipdown", uci.get("chillispot","settings","ipdown"),tr("chilli_var_dns2#IP Down Script"), "string")
		forms[#forms]:Add("text", "chillispot.settings.conup", uci.get("chillispot","settings","conup"),tr("chilli_var_conup#Conn.Up Script"), "string")
		forms[#forms]:Add("text", "chillispot.settings.condown", uci.get("chillispot","settings","condown"),tr("chilli_var_condown#Conn.Down Script"), "string")
--[[
# TUN parameters
# TAG: net
# IP network address of external packet data network
# Used to allocate dynamic IP addresses and set up routing.
# Normally you do not need to uncomment this tag.
#		option 'net' '192.168.182.0/24'

# TAG: dynip
# Dynamic IP address pool
# Used to allocate dynamic IP addresses to clients.
# If not set it defaults to the net tag.
# Do not uncomment this tag unless you are an experienced user!
#		option 'dynip' '192.168.182.0/24'

# TAG: statip
# Static IP address pool
# Used to allocate static IP addresses to clients.
# Do not uncomment this tag unless you are an experienced user!
#		option 'statip' '192.168.182.0/24'


# TAG: dns1
# Primary DNS server.
# Will be suggested to the client. 
# If omitted the system default will be used.
# Normally you do not need to uncomment this tag.
#		option 'dns1' '172.16.1.1'
		option 'dns1' '192.168.61.1'

# TAG: dns2
# Secondary DNS server.
# Will be suggested to the client.
# If omitted the system default will be used.
# Normally you do not need to uncomment this tag.
		option 'dns2' '172.16.1.2'

# TAG: domain
# Domain name
# Will be suggested to the client.
# Normally you do not need to uncomment this tag.
#		option 'domain' 'key.chillispot.org'

# TAG: ipup
# Script executed after network interface has been brought up.
# Executed with the following parameters: <devicename> <ip address>
# <mask>
# Normally you do not need to uncomment this tag.
#		option 'ipup' '/etc/chilli.ipup'

# TAG: ipdown
# Script executed after network interface has been taken down.
# Executed with the following parameters: <devicename> <ip address>
# <mask>
# Normally you do not need to uncomment this tag.
#		option 'ipdown' '/etc/chilli.ipdown'

# TAG: conup
# Script executed after a user has been authenticated.
# Executed with the following parameters: <devicename> <ip address>
# <mask> <user ip address> <user mac address> <filter ID>
# Normally you do not need to uncomment this tag.
#		option 'conup' '/etc/chilli.conup'

# TAG: condown
# Script executed after a user has disconnected.
# Executed with the following parameters: <devicename> <ip address>
# <mask> <user ip address> <user mac address> <filter ID>
# Normally you do not need to uncomment this tag.
#		option 'condown' '/etc/chilli.condown'
]]
	return forms
end

function radius()
	local forms = {}
	forms[#forms+1] = formClass.new("Config")
	forms[#forms]:Add("text", "chillispot.settings.radiuslisten", uci.get("chillispot","settings","radiuslisten"),tr("chilli_var_radiuslisten#Radius Listen"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusserver1", uci.get("chillispot","settings","radiusserver1"),tr("chilli_var_radiusserver1#Primary Radius Server"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusserver2", uci.get("chillispot","settings","radiusserver2"),tr("chilli_var_radiusserver2#Secondary Radius Server"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusauthport", uci.get("chillispot","settings","radiusauthport"),tr("chilli_var_radiusauthport#Authentication Port"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusacctport", uci.get("chillispot","settings","radiusacctport"),tr("chilli_var_radiusacctport#Accounting Port"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiussecret", uci.get("chillispot","settings","radiussecret"),tr("chilli_var_radiussecret#Radius Secret"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusnasid", uci.get("chillispot","settings","radiusnasid"),tr("chilli_var_radiusnasid#NAS Id"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusnasip", uci.get("chillispot","settings","radiusnasip"),tr("chilli_var_radiusnasip#NAS Ip"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiuscalled", uci.get("chillispot","settings","radiuscalled"),tr("chilli_var_radiuscalled#Radius Called"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiuslocationid", uci.get("chillispot","settings","radiuslocationid"),tr("chilli_var_radiuslocationid#Radius Location Id"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiuslocationname", uci.get("chillispot","settings","radiuslocationname"),tr("chilli_var_radiuslocationname#Radius Location Name"), "string")
	forms[#forms]:Add("text", "chillispot.settings.radiusnasporttype", uci.get("chillispot","settings","radiusnasporttype"),tr("chilli_var_radiusnasporttype#Radius Port Type"), "string")
	forms[#forms]:Add("text", "chillispot.settings.coaport", uci.get("chillispot","settings","coaport"),tr("chilli_var_coaport#COA Port"), "string")
	forms[#forms]:Add("text", "chillispot.settings.coanoipcheck", uci.get("chillispot","settings","coanoipcheck"),tr("chilli_var_coanoipcheck#COA Ip Check"), "string")
--[[
# Radius parameters

# TAG: radiuslisten
# IP address to listen to
# Normally you do not need to uncomment this tag.
#		option 'radiuslisten' '127.0.0.1'

# TAG: radiusserver1
# IP address of radius server 1
# For most installations you need to modify this tag.
#		option 'radiusserver1' 'rad01.chillispot.org'
		option 'radiusserver1' '172.16.1.2'

# TAG: radiusserver2
# IP address of radius server 2
# If you have only one radius server you should set radiusserver2 to the
# same value as radiusserver1.
# For most installations you need to modify this tag.
#		option 'radiusserver2' 'rad02.chillispot.org'
#		option 'radiusserver2' '172.16.1.2'

# TAG: radiusauthport
# Radius authentication port
# The UDP port number to use for radius authentication requests.
# The same port number is used for both radiusserver1 and radiusserver2.
# Normally you do not need to uncomment this tag.
#		option 'radiusauthport' '1812'

# TAG: radiusacctport
# Radius accounting port
# The UDP port number to use for radius accounting requests.
# The same port number is used for both radiusserver1 and radiusserver2.
# Normally you do not need to uncomment this tag.
#		option 'radiusacctport' '1813'

# TAG: radiussecret
# Radius shared secret for both servers
# For all installations you should modify this tag.
		option 'radiussecret' 'InternetWifi'

# TAG: radiusnasid
# Radius NAS-Identifier
# Normally you do not need to uncomment this tag.
#		option 'radiusnasid' 'nas01'

# TAG: radiusnasip
# Radius NAS-IP-Address
# Normally you do not need to uncomment this tag.
#		option 'radiusnasip' '127.0.0.1'

# TAG: radiuscalled
# Radius Called-Station-ID
# Normally you do not need to uncomment this tag.
#		option 'radiuscalled' '00133300'

# TAG: radiuslocationid
# WISPr Location ID. Should be in the format: isocc=<ISO_Country_Code>,
# cc=<E.164_Country_Code>,ac=<E.164_Area_Code>,network=<ssid/ZONE>
# Normally you do not need to uncomment this tag.
#		option 'radiuslocationid' 'isocc=us,cc=1,ac=408,network=ACMEWISP_NewarkAirport'

# TAG: radiuslocationname
# WISPr Location Name. Should be in the format: 
# <HOTSPOT_OPERATOR_NAME>,<LOCATION>
# Normally you do not need to uncomment this tag.
#		option 'radiuslocationname' 'ACMEWISP,Gate_14_Terminal_C_of_Newark_Airport'

# TAG: radiusnasporttype
# Value of NAS-Port-Type attribute. Defaults to 19 (Wireless-IEEE-802.11). 
#		option 'radiusnasporttype' '19'

# TAG: coaport
# UDP port to listen to for accepting radius disconnect requests. 
#		option 'coaport' 'port' 

# TAG: coanoipcheck
# If this option is given no check is performed on the source IP address of radius disconnect requests. Otherwise it is checked that radius disconnect requests originate from radiusserver1 or radiusserver2. 
#		option	'coanoipcheck' '0'

]]
	return forms
end

function proxy()
	local forms = {}
	forms[#forms+1] = formClass.new("Proxy Settings")
	forms[#forms]:Add("text", "chillispot.settings.proxylisten", uci.get("chillispot","settings","proxylisten"),tr("chilli_var_proxylisten#Proxy Listen"), "string")
	forms[#forms]:Add("text", "chillispot.settings.proxyport", uci.get("chillispot","settings","proxyport"),tr("chilli_var_proxyport#Proxy Port"), "string")
	forms[#forms]:Add("text", "chillispot.settings.proxyclient", uci.get("chillispot","settings","proxyclient"),tr("chilli_var_proxyclient#Proxy Client"), "string")
	forms[#forms]:Add("text", "chillispot.settings.proxysecret", uci.get("chillispot","settings","proxysecret"),tr("chilli_var_proxysecret#Proxy Secret"), "string")
	forms[#forms]:Add("text", "chillispot.settings.confusername", uci.get("chillispot","settings","confusername"),tr("chilli_var_confusername#Conf Username"), "string")
	forms[#forms]:Add("text", "chillispot.settings.confpassword", uci.get("chillispot","settings","confpassword"),tr("chilli_var_confpassword#Conf Password"), "string")
--[[
# Radius proxy parameters

# TAG: proxylisten
# IP address to listen to
# Normally you do not need to uncomment this tag.
#		option 'proxylisten' '10.0.0.1'

# TAG: proxyport
# UDP port to listen to. 
# If not specified a port will be selected by the system
# Normally you do not need to uncomment this tag.
#		option 'proxyport' '1645'

# TAG: proxyclient
# Client(s) from which we accept radius requests
# Normally you do not need to uncomment this tag.
#		option 'proxyclient' '10.0.0.1/24'

# TAG: proxysecret
# Radius proxy shared secret for all clients
# If not specified defaults to radiussecret
# Normally you do not need to uncomment this tag.
#		option 'proxysecret' 'testing123'

# TAG: confusername
# If confusername is specified together with confpassword chillispot
# will at regular intervals specified by the interval option query the
# radius server for configuration information.
# The reply from the radius server must have the Service-Type attribute set to 
# ChilliSpot-Authorize-Only in order to have any effect. 
# Currently ChilliSpot-UAM-Allowed, ChilliSpot-MAC-Allowed and 
# ChilliSpot-Interval is supported. These attributes override the uamallowed ,
# macallowed and interval options respectively. 
# Normally you do not need to uncomment this tag.
#		option 'confusername' 'conf'

# TAG: confpassword
# If confusername is specified together with confpassword chillispot
# will at regular intervals specified by the interval option query the
# radius server for configuration information.
# Normally you do not need to uncomment this tag.
#		option 'confpassword' 'secret'
]]
	return forms
end

function dhcp()
	local forms = {}
	forms[#forms+1] = formClass.new("DHCP Settings")
	forms[#forms]:Add("select","chillispot.settings.dhcpif",uci.get("chillispot","settings","dhcpif"),tr("chilli_var_dhcpif#Chilli Interface"),"string")
	for k, v in pairs(interfaces()) do
		forms[#forms]["chillispot.settings.dhcpif"].options:Add(v,k)
	end
	forms[#forms]:Add("text", "chillispot.settings.dhcpmac", uci.get("chillispot","settings","dhcpmac"),tr("chilli_var_dhcpmac#DHCP MAC"), "string")
	forms[#forms]:Add("text", "chillispot.settings.lease", uci.get("chillispot","settings","lease"),tr("chilli_var_lease#Lease Time"), "string")
	forms[#forms]:Add("text", "chillispot.settings.eapolenable", uci.get("chillispot","settings","eapolenable"),tr("chilli_var_eapolenable#IEEE 802.1x authentication"), "string")
	forms[#forms]:Add("select","chillispot.settings.eapolenable",uci.get("chillispot","settings","eapolenable"),tr("chilli_var_eapolenable#IEEE 802.1x authentication"),"string")
	forms[#forms]["chillispot.settings.eapolenable"].options:Add("0","Disable")
	forms[#forms]["chillispot.settings.eapolenable"].options:Add("1","Enable")
--[[
# DHCP Parameters
# TAG: dhcpif
# Ethernet interface to listen to.
# This is the network interface which is connected to the access points.
# In a typical configuration this tag should be set to eth1.
		option 'dhcpif' 'br-wifi'

# TAG: dhcpmac
# Use specified MAC address.
# MAC address to listen to. If not specified the MAC address of the interface
# will be used. The MAC address should be chosen so that it does not conflict
# with other addresses on the LAN.
# An address in the range 00:00:5E:00:02:00 - 00:00:5E:FF:FF:FF falls
# within the IANA range of addresses and is not allocated for other purposes. 
# The --dhcpmac option can be used in conjunction with access filters in the
# access points, or with access points which supports packet forwarding to a
# specific MAC address. Thus it is possible at the MAC level to separate access
# point management traffic from user traffic for improved system security. 
#
# The --dhcpmac option will set the interface in promisc mode. 
# Normally you do not need to uncomment this tag.
#		option 'dhcpmac' '00:00:5E:00:02:00'

# TAG: lease
# Time before DHCP lease expires
# Normally you do not need to uncomment this tag.
#		option 'lease' '600'

# TAG: eapolenable
# If this option is given IEEE 802.1x authentication is enabled.
# ChilliSpot will listen for EAP authentication requests on the interface
# specified by --dhcpif.
# EAP messages received on this interface are forwarded to the radius server. 
#		option 'eapolenable' '0'

]]
	return forms
end

function uam()
	local forms = {}
	forms[#forms+1] = formClass.new("UAM Settings")
	forms[#forms]:Add("text", "chillispot.settings.uamserver", uci.get("chillispot","settings","uamserver"),tr("chilli_var_uamserver#Server"), "string")
	forms[#forms]:Add("text", "chillispot.settings.uamhomepage", uci.get("chillispot","settings","uamhomepage"),tr("chilli_var_uamhomepage#Home Page"), "string")
	forms[#forms]:Add("text", "chillispot.settings.uamsecret", uci.get("chillispot","settings","uamsecret"),tr("chilli_var_uamsecret#Secret"), "string")
	forms[#forms]:Add("text", "chillispot.settings.uamlisten", uci.get("chillispot","settings","uamlisten"),tr("chilli_var_uamlisten#Listen"), "string")
	forms[#forms]:Add("text", "chillispot.settings.uamport", uci.get("chillispot","settings","uamport"),tr("chilli_var_uamport#Port"), "string")
--	forms[#forms]:Add("text", "chillispot.settings.uamallowed", uci.get("chillispot","settings","uamallowed"),tr("chilli_var_uamallowed#Allowed"), "string")
	forms[#forms]:Add("list_add", "chillispot.settings.uamallowed", uci.get("chillispot","settings","uamallowed"),tr("chilli_var_uamallowed#Allowed"), "string", "width: 100%")
	forms[#forms]:Add("select","chillispot.settings.uamanydns",uci.get("chillispot","settings","uamanydns"),tr("chilli_var_uamanydns#Any DNS"),"string")
	forms[#forms]["chillispot.settings.uamanydns"].options:Add("0","Disable")
	forms[#forms]["chillispot.settings.uamanydns"].options:Add("1","Enable")
--[[
# Universal access method (UAM) parameters

# TAG: uamserver
# URL of web server handling authentication.
#		option 'uamserver' 'https://radius.chillispot.org/hotspotlogin'
#		option 'uamserver' 'http://192.168.182.1/cgi-bin/login/login'
		option 'uamserver' 'http://www.internet-wifi.com.ar/hotspotlogin_m.php'

# TAG: uamhomepage
# URL of welcome homepage.
# Unauthenticated users will be redirected to this URL. If not specified
# users will be redirected to the uamserver instead.
# Normally you do not need to uncomment this tag.
#		option 'uamhomepage' 'http://192.168.182.1/welcome.html'

# TAG: uamsecret
# Shared between chilli and authentication web server
#		option 'uamsecret' 'ht2eb8ej6s4et3rg1ulp'
		option 'uamsecret' 'InternetWifi'

# TAG: uamlisten
# IP address to listen to for authentication requests
# Do not uncomment this tag unless you are an experienced user!
#		option 'uamlisten' '192.168.182.1'

# TAG: uamport
# TCP port to listen to for authentication requests
# Do not uncomment this tag unless you are an experienced user!
#		option 'uamport' '3990'

# TAG: uamallowed
# Comma separated list of domain names, IP addresses or network segments
# the client can access without first authenticating.
# Normally you do not need to uncomment this tag.
#		option 'uamallowed' 'www.chillispot.org,10.11.12.0/24'
	option 'uamallowed' '172.16.1.2,172.16.1.2,www.google.com,10.0.0.0/8'
	
# TAG: uamanydns
# If this flag is given unauthenticated users are allowed to use
# any DNS server.
# Normally you do not need to uncomment this tag.
#		option 'uamanydns' '0'
]]
	return forms
end

function mac()
	local forms = {}
	forms[#forms+1] = formClass.new("MAC Authentication Settings")
	forms[#forms]:Add("select","chillispot.settings.macauth",uci.get("chillispot","settings","macauth"),tr("chilli_var_macauth#MAC Authentication"),"string")
	forms[#forms]["chillispot.settings.macauth"].options:Add("0","Disable")
	forms[#forms]["chillispot.settings.macauth"].options:Add("1","Enable")
	forms[#forms]:Add("list_add", "chillispot.settings.macallowed", uci.get("chillispot","settings","macallowed"),tr("chilli_var_macallowed#MAC Allowed"), "string")
	forms[#forms]:Add("text", "chillispot.settings.macpassword", uci.get("chillispot","settings","macpassword"),tr("chilli_var_macpassword#Mac Password"), "string")
	forms[#forms]:Add("text", "chillispot.settings.macsuffix", uci.get("chillispot","settings","macsuffix"),tr("chilli_var_macsuffix#Mac Suffix"), "string")
--[[
# MAC authentication

# TAG: macauth
# If this flag is given users will be authenticated only on their MAC
# address.
# Normally you do not need to uncomment this tag.
#		option 'macauth' '0'

# TAG: macallowed
# List of MAC addresses.
# The MAC addresses specified in this list will be authenticated only on
# their MAC address.
# The User-Name sent to the radius server will consist of the MAC address and 
# an optional suffix which is specified by the macsuffix option. 
# If the macauth option is specified the macallowed option is ignored. 
# It is possible to specify the macallowed option several times.
# This is useful if many mac addresses has to be specified. 
# This tag is ignored if the macauth tag is given.
# Normally you do not need to uncomment this tag.
#		option 'macallowed' '00-0A-5E-AC-BE-51,00-30-1B-3C-32-E9,00-18-DE-26-E8-35'

# TAG: macpasswd
# Password to use for MAC authentication.
# Normally you do not need to uncomment this tag.
#		option 'macpasswd' 'password'

# TAG: macsuffix
# Suffix to add to MAC address in order to form the User-Name,
# which is sent to the radius server. 
# Normally you do not need to uncomment this tag.
#		option 'macsuffix' 'suffix'
]]
	return forms
end

function socket()
	local forms = {}
	forms[#forms+1] = formClass.new("Socket Setting")
	forms[#forms]:Add("text", "chillispot.settings.rmtlisten", uci.get("chillispot","settings","rmtlisten"),tr("chilli_var_rmtlisten#Listen"), "string")
	forms[#forms]:Add("text", "chillispot.settings.rmtport", uci.get("chillispot","settings","rmtport"),tr("chilli_var_rmtport#Port"), "string")
	forms[#forms]:Add("text", "chillispot.settings.rmtpassword", uci.get("chillispot","settings","rmtpassword"),tr("chilli_var_rmtpassword#Password"), "string")
--[[
# TAG: rmtlisten
# IP address to listen to for remote monitor and config
# Do not uncomment this tag unless you are an experienced user!
#		option 'rmtlisten' '127.0.0.1'

# TAG: rmtport
# TCP port to listen to for remote monitor and config
# Do not uncomment this tag unless you are an experienced user!
		option 'rmtport' '3991'

# TAG: rmtpasswd
# Password to use for remote config by socket.
# Normally you do not need to uncomment this tag.
#		option 'rmtpasswd' 'rmtpassword'

]]
	return forms
end