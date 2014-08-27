--Create Form
m = SimpleForm("macclone", translate("MAC Address Clone"),
	translate("Some ISPs will require you to register your MAC address. If you do not wish to re-register your MAC address, you can have the router clone the MAC address that is registered with your ISP."))
s = m:section(SimpleSection)

--Translate submit button to "Clone Mac...", hidden reset button
m.submit = translate("Clone Mac...")
m.reset  = false

--Show texbox "Curent WAN MAC"
curentmac             = s:option(Value, "curentmac", translate("Current WAN MAC"))
curentmac.datatype    = "macaddr"
curentmac.placeholder = "00:00:00:00:00:00"

--Show button "Restore Original MAC Address"
ori_mac            = s:option(Button, "ori_mac", translate(" "))
ori_mac.inputtitle = translate("Restore Original MAC Address")
ori_mac.inputstyle = "apply"
ori_mac.rmempty    = false

--Get WAN interface name
cmd    = "uci -P/var/state get network.wan.ifname"
f      = io.popen(cmd)
wan_if = f:read()
f:close()

--Get current MAC
cmd     = "uci show network.wan | grep .macaddr | awk -F'=' '{print $2}'"
f       = io.popen(cmd)
cur_mac = f:read()
f:close()
if cur_mac == nil then
    cmd     =  "ifconfig "..wan_if.." | grep HWaddr | awk -F'HWaddr ' '{print $2}'"
    f       = io.popen(cmd)
    cur_mac = f:read()
	if cur_mac then
		curentmac:value("", cur_mac.."(Original MAC)")
	end
    f:close()
else
    curentmac:value("", cur_mac.."(Has changed)")
end

--Mac Clone
function curentmac.write(self)
	local mac = luci.http.formvalue("cbid.macclone.1.curentmac")
	if mac and mac ~= cur_mac then
		cmd       = "uci set network.wan.macaddr="..mac.." && uci commit network && /etc/init.d/network restart"
		p         = io.popen(cmd)
		p:close()			
		m.message = "Clone MAC("..mac..") successfully"
		cur_mac   = mac
		curentmac:value("", mac.."(Has changed)")
    else
        m.message     = "Clone MAC unsuccessfully"
	end
end

--Restore original MAC
function ori_mac.write(self)
	--Check macaddr set to /etc/config/network
    cmd           = "uci show network.wan | grep .macaddr | awk -F'=' '{print $2}'"
    local p       = io.popen(cmd)
    local mac     = p:read()
    p:close()
    
    if mac then
		--Remove macaddr from /etc/config/network
        cmd       = "uci delete network.wan.macaddr && uci commit network && /etc/init.d/network restart"
        p         = io.popen(cmd)
        p:close()
 
        --Get original Mac
        cmd           =  "ifconfig "..wan_if.." | grep HWaddr | awk -F'HWaddr ' '{print $2}'"
        p             = io.popen(cmd)
        local ori_mac = p:read()
		p:close()
        
		--Show Mac to text box
		if ori_mac then
			m.message = "Restore original MAC successfully"
			curentmac:value("", ori_mac.."(Original MAC)")
            cur_mac   = ori_mac
		else
			m.message = "Restore original MAC unsuccessfully"
		end
    else
        m.message     = "Current Mac is original"
	end
end

return m
