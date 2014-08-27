require("lua-xwrt.addon.string")
require("lua-xwrt.addon.uci")
require("lua-xwrt.html.form")
require("lua-xwrt.xwrt.translator")
require("lua-xwrt.addon.io")
ssl = require("lua-xwrt.openssl")

for u, v in pairs(__FORM) do
--	print(u,v,"<br>")
	local proc, uci_var, uci_cmd, idx, uci_val = unpack(string.split(u,":"))
	if uci_var ~= nil and uci_cmd ~= nil then
--	print(proc, uci_var, uci_cmd, idx, uci_val,"<br>")
		if proc == "cert" then 
			if uci_val == "server" and uci.get("openvpn",uci_var,"mode") then
				if uci_cmd == "del" or uci_cmd == "revoke" then
					uci.delete("openvpn", uci_var, "cert")
					uci.delete("openvpn", uci_var, "key")
					uci.save("openvpn")
					uci.commit("openvpn")
				end
			end
		end
		if proc == "srv_cert" then
			if uci_cmd == "new" then
				if uci_val == "server" then
					uci.check_set("certificate","newcert","entity")
					uci.set("certificate","newcert","countryName", uci.get("openssl",uci_var,"poly_match_countryName_default"))
					uci.set("certificate","newcert","stateName", uci.get("openssl",uci_var, "poly_match_stateOrProvinceName_default" ))
					uci.set("certificate","newcert","localityName",uci.get("openssl",uci_var, "poly_match_localityName_default"))
					uci.set("certificate","newcert","organizationName",uci.get("openssl",uci_var,"poly_match_organizationName_default"))
					uci.set("certificate","newcert","organizationUnitName",uci.get("openssl",uci_var,"poly_match_organizationUnitName_default"))
					uci.set("certificate","newcert","emailAddress",uci.get("openssl",uci_var,"poly_match_emailAddress_default"))
					uci.set("certificate","newcert","commonName", "server")
					uci.save("certificate")
					__FORM[u] = nil
					ssl.form_progress("/etc/openssl/cert-build "..uci_var.." server","Creating Certificate","Server Certificate and Key")
					uci.delete("certificate","newcert")
					uci.save("certificate")
					uci.commit("certificate")
--					os.execute("rm /var/.uci/certificate")
					if  uci.get("openvpn",uci_var,"mode") then
						uci.set("openvpn",uci_var,"cert","/etc/openssl/"..uci_var.."/certs/server.crt")
						uci.set("openvpn",uci_var,"key","/etc/openssl/"..uci_var.."/private/server.key")
						uci.save("openvpn")
						uci.commit("openvpn")
					end
					os.exit(0)
				elseif idx == "ta.key" then
					__FORM[u] = nil
					ssl.form_progress("openvpn --genkey --secret /etc/openssl/"..uci_var.."/ta.key","Creating Key","ta.key")
					uci.set("openvpn",uci_var,"tls_auth","/etc/openssl/"..uci_var.."/ta.key")
					uci.save("openvpn")
					uci.commit("openvpn")
					os.exit(0)
				end
			elseif uci_cmd == "del" then
				if idx == "ta.key" then
					os.execute("rm /etc/openssl/"..uci_var.."/ta.key")
					uci.delete("openvpn",uci_var,"tls_auth")
					uci.save("openvpn")
					uci.commit("openvpn")
				end
			end
		end
		if proc == "cert_list" then
			local path = uci.get("openvpn",__FORM.name,"path")
			os.execute("rm "..path..uci_val)
		elseif proc == "service" then -- Este va a general
			os.execute("/etc/init.d/"..uci_var.." "..uci_cmd)
			if uci_cmd == "close" then
				os.execute("killall "..uci_var)
			end
			os.execute("sleep 3")
		elseif proc == "dir_list" then 
			uci.delete(uci_var,uci_val)
			uci.save(uci_var)
			uci.commit(uci_var)
			uci.delete("openssl",uci_val)
			uci.save("openssl")
			uci.commit("openssl")
			os.execute("rm -rf /etc/openvpn/"..uci_val)
			os.execute("rm -rf /etc/openssl/"..uci_val)
		elseif proc == "uci_list" then
			idx = tonumber(idx)
			if uci_cmd then
				local c,s,o = unpack(string.split(uci_var,"."))
				local ulist = uci.get(c, s, o) or {}
				if uci_cmd == "add" and v ~= "" then
--print(u,c,s,o,"(",v,")",__FORM[u])				
					table.insert(ulist,v)
					uci.set(c,s,o,ulist)
					uci.save(c)
					uci.commit(c)
				elseif uci_cmd == "set" then
--print(u,c,s,o,"(",v,")",__FORM[u])
					if v ~= "" and ulist[idx] ~= v then
						ulist[idx] = v
						uci.set(c,s,o,ulist)
						uci.save(c)
						uci.commit(c)
					end
				elseif uci_cmd == "del" then
--					for a,s in ipairs(ulist) do
--						print(a,s,"<br>")
--					end
--					print(ulist[idx],uci_val, idx, "<br>")
--print("uci_list:",c,".",s,".",o,":set:",idx)
					if ulist[idx] == uci_val then
						__FORM["uci_list:"..c.."."..s.."."..o..":set:"..idx] = nil
						table.remove(ulist,idx)
						uci.set(c, s, o, ulist)
						uci.save(c)
						uci.commit(c)
					end
				end
			end
		elseif proc == "ucifile" then
			local c,s,o = unpack(string.split(uci_var,"."))
			local path = "/etc/openvpn/"..s.."/"
--print(uci_cmd,c,s,o,path)			
			if uci_cmd == "set" and v.filename ~= "" and v.data ~= "" then
--print(v.filename,path)			
				os.execute("mkdir -p "..path)
				uci.set(c,s,o,path..v.filename)
				uci.save(c)
				uci.commit(c)
				local file = io.open(path..v.filename,"w")
				file:write(v.data)
				file:close()
			elseif uci_cmd == "del" then
				uci.delete(c,s,o)
				uci.save(c)
				uci.commit(c)
			end
		end
	end
end
--print("terminó con las listas")
--print("New","<br>")

if __FORM.openvpn_new_name and __FORM.openvpn_new_name ~= "" then
	uci.check_set("openvpn",__FORM.openvpn_new_name,"openvpn")
	local path = "/etc/openvpn/"..__FORM.openvpn_new_name.."/"
	os.execute("mkdir -p "..path)
	if __FORM.openvpn_new_type == "client" then
		uci.set("openvpn",__FORM.openvpn_new_name,"client","1")
	elseif __FORM.openvpn_new_type == "tls_client" then
		uci.set("openvpn",__FORM.openvpn_new_name,"tls_client","1")
	elseif __FORM.openvpn_new_type == "server" then
		ssl.new_entity(__FORM.openvpn_new_name)
		uci.set("openvpn",__FORM.openvpn_new_name,"mode","server")
		os.execute("/etc/openvpn/clear-all "..__FORM.openvpn_new_name)
	elseif __FORM.openvpn_new_type == "custom" then
		ssl.new_entity(__FORM.openvpn_new_name)
		local file = io.open(path.."openvpn.conf","w")
		file:write("#### "..__FORM.openvpn_new_name.." ####\n#### You must write propper configuration ####")
		file:close()
		uci.set("openvpn",__FORM.openvpn_new_name,"config",path.."openvpn.conf")
	end
	__FORM.option = __FORM.openvpn_new_type
	__FORM.name = __FORM.openvpn_new_name
	for k, v in pairs(__FORM) do
		if tr(string.unescape(v)) == "Core" then
			__FORM[k] = __FORM.openvpn_new_name
		end
	end
--	__FORM.suboption = "cert_setting"
	uci.set("openvpn",__FORM.openvpn_new_name,"enable","0")
	uci.save("openvpn")
	uci.commit("openvpn")
end

if __FORM.uploadfile and __FORM.name and __FORM.uploadfile.filename ~= "" then
	local path = uci.get("openvpn",__FORM.name,"path")
	local l = string.len(uci.get("openvpn",__FORM.name,"path"))
	if string.sub(path,l) ~= "/" then
		path = path.."/"
		uci.set("openvpn",__FORM.name,"path",path)
		uci.save("openvpn")
		uci.commit("openvpn")
	end
	os.execute("mkdir -p "..path)
	file = io.open(path..__FORM.uploadfile.filename,"w")
	file:write(__FORM.uploadfile.data)
	file:close()
end
	
if __FORM.openvpn_file_custom_config and __FORM.name then
	file = io.open(uci.get("openvpn",__FORM.name,"config"),"w")
	file:write(__FORM.openvpn_file_custom_config)
	file:close()
--	os.execute("echo '"..__FORM.openvpn_file_custom_config.."' > "..uci.get("openvpn",__FORM.name,"config"))
end

local string = string
local io = io
local os = os
local pairs, ipairs = pairs, ipairs
local table = table
local uci = uci
local util = util
local string = string
local type = type
local print = print
local tostring, tonumber = tostring, tonumber
local formClass = formClass
local htmlhmenuClass = htmlhmenuClass
local tr = tr
local __MENU = __MENU
local __FORM = __FORM
local ssl = ssl

--print("crea el modulo<br>")
module("lua-xwrt.openvpn")
local tconf = {}
local newMenu = htmlhmenuClass.new("submenu")

function set_menu(t)
	local sub = ""
	local mode = "" 
	if t.config then
		sub = "openvpn.sh?option=custom&name="..t[".name"]
		mode = "custom"
	elseif t.client or t.tls_client then
		sub = "openvpn.sh?option=client&name="..t[".name"]
		mode = "client"
	elseif t.mode == "server" then
		mode = "server"
		sub = "openvpn.sh?option=server&name="..t[".name"]
	else
		mode = "server"
		sub = "openvpn.sh?option=server&name="..t[".name"]
	end
	newMenu:add(t[".name"],sub)
	if t[".name"] == __FORM.name then
		newMenu[t[".name"]] = htmlhmenuClass.new("submenu")
		
		if mode == "server" then
			newMenu[t[".name"]]:add("Server",sub.."&suboption=server")
			newMenu[t[".name"]]:add("CA default",sub.."&suboption=cert_setting&setting=ca_def")
			newMenu[t[".name"]]:add("Policy Match",sub.."&suboption=cert_setting&setting=policy_match")
			newMenu[t[".name"]]:add("Server Certificates",sub.."&suboption=srv_cert")
			if util.file_exists("/etc/openssl/"..__FORM.name.."/cacert.crt") then
				newMenu[t[".name"]]:add("Client Certificates", sub.."&suboption=certificates")
			end
		elseif mode == "custom" then
			newMenu[t[".name"]]:add("Configuration",sub.."&suboption=config")
			newMenu[t[".name"]]:add("Upload Certificates",sub.."&suboption=cli_certificates")
			newMenu[t[".name"]]:add("Certificates Setting",sub.."&suboption=cert_setting")
			newMenu[t[".name"]]["Certificates Setting"] = htmlhmenuClass.new("submenu")
			newMenu[t[".name"]]["Certificates Setting"]:add("CA default",sub.."&suboption=cert_setting&setting=ca_def")
			newMenu[t[".name"]]["Certificates Setting"]:add("Policy Match",sub.."&suboption=cert_setting&setting=policy_match")
				newMenu[t[".name"]]["Certificates Setting"]:add("Server Certificates", sub.."&suboption=srv_cert")
			if util.file_exists("/etc/openssl/"..__FORM.name.."/cacert.crt") then
				newMenu[t[".name"]]["Certificates Setting"]:add("Certificates", sub.."&suboption=certificates")
			end
		else
					newMenu[t[".name"]]:add("Tunnel",sub.."&suboption=cli_setting")
					newMenu[t[".name"]]:add("Upload Certificates",sub.."&suboption=cli_certificates")
		end
	end
--print("end_menu")
--	print(util.table2string(t,"<br>"))	
end

function init()
--print("init")
	newMenu:add("openvpn_menu_Core#Core","openvpn.sh")
	uci.foreach("openvpn","openvpn", set_menu)
	if __MENU[__FORM.cat].len > 1 then 
		__MENU[__FORM.cat]["OpenVpn"] = newMenu
	else
		__MENU[__FORM.cat] = newMenu
	end
end

function form_new(form)
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openvpn_core#Core Service"))
  else
    form:Add("subtitle",tr("openvpn_core#Core Service"))
  end

	form:Add("service","openvpn.service.enable","openvpn",tr("openvpn_var_service#Service"),"")
	form["openvpn.service.enable"].options:Add("service","openvpn")
	form["openvpn.service.enable"].options:Add("init","openvpn")

	form:Add("subtitle",tr("openvpn_new_conf#Create New Configuration"))
	form:Add("select","openvpn_new_type","client",tr("openvpn_new_type#Tunnel type"),"string")
	if ssl then
		form["openvpn_new_type"].options:Add("server",tr("Server"))
	end
	form["openvpn_new_type"].options:Add("client",tr("Client"))
	form["openvpn_new_type"].options:Add("tls_client",tr("TLS-Client"))
	form["openvpn_new_type"].options:Add("custom",tr("Custom"))
	form:Add("text", "openvpn_new_name", "",tr("openvpn_var_new_name#Tunnel Name"),"string")
	form:Add_help(tr("openvpn_var_new_type#Configuration Type"),tr("openvpn_help_new_type#<strong>Client:</strong> To configure openvpn Client in uci file<br><strong>Server:</strong> To configure opnevpn Server in uci file<br><strong>Custom:</strong> You write openvpn.conf file to Server or Client in openvpn native way.<br><br><strong>More Help:</strong><ul>Exernal Links:<li><a href='http://openvpn.net/index.php/open-source/documentation/manuals/69-openvpn-21.html' target='_externLink'>OpenVPN site</a></li><li><a href='http://wiki.openwrt.org/oldwiki/openvpntunhowto' target='_externLink'>OpenWRT Wiki</a></li></ul>"))
	form:Add_help_link('http://openvpn.net/index.php/open-source/documentation/manuals/69-openvpn-21.html',"OpenVPN Site", true)
	form:Add("subtitle",tr("openvpn_del_conf#Remove Configuration"))
	
	local dir = util.dirList("/etc/openvpn")
	if ssl then
		dir = ssl.joinDirUci(dir,uci.get_type("openvpn","openvpn"))
	end
	form:Add("list", "dir_list:openvpn", dir, tr("Tunnel Names List"),"","","",true)
	forms[#forms+1] = form
	return forms
end

function form_custom(form,name)
	if __FORM.suboption == "cli_certificates" then
		return form_custom_cert(nil,name)
	elseif __FORM.suboption == "cert_setting" then
--		if io.exists("/etc/openssl/"..name.."/serial") == false then
--			ssl.new_entity(name)
--		end
		return ssl.form_entity_settings(form,name)
	elseif __FORM.suboption == "srv_cert" then
		if io.exists("/etc/openssl/"..name.."/serial") == false then
			ssl.new_entity(name)
		end
		return form_serverCert(form,name)
--		return ssl.form_entity_ca(form,name)
	elseif __FORM.suboption == "certificates" then
		if io.exists("/etc/openssl/"..name.."/serial") == false then
			ssl.new_entity(name)
		end
		return form_clients_crts(form,name)
	else
		return form_config(form,name)
	end
end

function form_config(form,name)
	local forms ={}
	local certs = {}
  form1 = formClass.new(tr("openvpn_custom_type#Custom").." - "..name,true)
	local conf_data, len = util.file_load(uci.get("openvpn",name,"config"))
	form1:Add("select","openvpn."..name..".enable",uci.get("openvpn",name,"enable"),tr("openvpn_service#Service"),"string")
	form1["openvpn."..name..".enable"].options:Add("0","Disable")
	form1["openvpn."..name..".enable"].options:Add("1","Enable")
  form1:Add("text_area", "openvpn_file_custom_config", conf_data, uci.get("openvpn",name,"config"), "string", "float:right;width:100%;height:300px;")
	local path = "/etc/openvpn/"..name.."/"
	fcerts = io.popen("ls "..path)
	for line in fcerts:lines() do
		if line ~= "openvpn.conf" then
			certs[#certs+1] = path..line
		end
	end
	forms[#forms+1] = form1
--	forms[#forms+1] = form_custom_cert(nil,name)
--	forms[#forms]:Add("list", "cert_list", certs, tr("Files List"),"","","",true)
	return forms
end

function form_clients_crts(form,name)
	local name = name
	local form = form
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openvpn_client_crts#Client Certificates of").." "..name,true)
  else
    form:Add("subtitle",tr("openvpn_client_crts#Client Certificates of").." "..name)
  end
	local varType = uci.get("openssl",name,"poly_match_countryName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.countryName",uci.get("openssl","newcert","countryName"), tr("openssl_var_countryName#Country Name"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.countryName",uci.get("openssl",name,"poly_match_countryName_default"), tr("openssl_var_countryName#Country Name"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.countryName",uci.check_set("openssl",name,"poly_match_countryName_default",""), tr("openssl_var_countryName#Country Name"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_stateOrProvinceName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.stateName",uci.get("openssl","newcert","stateName"), tr("openssl_var_stateName#State or Province Name"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.stateName",uci.get("openssl",name,"poly_match_stateOrProvinceName_default"), tr("openssl_var_stateName#State or Province Name"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.stateName",uci.check_set("openssl",name,"poly_match_stateOrProvinceName_default",""), tr("openssl_var_stateName#State or Province Name"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_localityName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.localityName",uci.get("openssl","newcert","localityName"), tr("openssl_var_localityName#Locality Name"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.localityName",uci.get("openssl",name,"poly_match_localityName_default"), tr("openssl_var_localityName#Locality Name"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.localityName",uci.check_set("openssl",name,"poly_match_localityName_default",""), tr("openssl_var_localityName#Locality Name"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_organizationName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.organizationName",uci.get("openssl","newcert","organizationName"), tr("openssl_var_organizationName#Organization Name"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.organizationName",uci.get("openssl",name,"poly_match_organizationName_default"), tr("openssl_var_organizationName#Organization Name"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.organizationName",uci.check_set("openssl",name,"poly_match_organizationName_default",""), tr("openssl_var_organizationName#Organization Name"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_organizationUnitName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.organizationUnitName",uci.get("openssl","newcert","organizationUnitName"), tr("openssl_var_organizationUnitName#Organization Unit"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.organizationUnitName",uci.get("openssl",name,"poly_match_organizationUnitName_default"), tr("openssl_var_organizationUnitName#Organization Unit"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.organizationUnitName",uci.check_set("openssl",name,"poly_match_organizationUnitName_default",""), tr("openssl_var_organizationUnitName#Organization Unit"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_emailAddress") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.emailAddress",uci.get("openssl","newcert","emailAddress"), tr("openssl_var_emailAddress#E-Mail Address"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.emailAddress",uci.get("openssl",name,"poly_match_emailAddress_default"), tr("openssl_var_emailAddress#E-Mail Address"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.emailAddress",uci.check_set("openssl",name,"poly_match_emailAddress_default",""), tr("openssl_var_emailAddress#E-Mail Address"),"string")
	end
	varType = uci.get("openssl",name,"poly_match_commonName") 
	if varType == "supplied" then
		form:Add("text","certificate.newcert.commonName",uci.get("openssl","newcert","commonName"), tr("openssl_var_commonName#Common Name"),"string")
	elseif varType == "match" then
		form:Add("hidden","certificate.newcert.commonName",uci.get("openssl",name,"poly_match_commonName_default"), tr("openssl_var_commonName#Common Name"),"string")
	elseif varType == "optional" then
		form:Add("hidden","certificate.newcert.commonName",uci.check_set("openssl",name,"poly_match_commonName_default",""), tr("openssl_var_commonName#Common Name"),"string")
	end
	form:Add("submit","cert:"..name..":new","Create Certificate",tr("openssl_var_newCert#New Certificate"),"", "width:147px;")
	local crtsList = ssl.readSslDb(name)
	local filetb = ssl.file_list("/etc/openssl/"..name.."/certs/*")
	local t = {}
	local srvFile = check_srvFile(name,"/certs/server.crt")
	local str = ""
	if #filetb > 0 then
		for k, file in ipairs(filetb) do
--print("for", file, "check", srvFile)
			if file ~= srvFile then
				ssl.sslTableInfo(file,"-dates", t, crtsList)
			end
		end
		str = str .. ssl.htmlTableCerts(t,name)
	end
	form:Add("text_line","linea1",str)
	forms[#forms+1] = form
	return forms
end
	
function form_server(form,name)
	if __FORM.suboption == "certificates" then
		return form_clients_crts(form,name)
	elseif __FORM.suboption == "ca" then
		return ssl.form_entity_ca(form,name)
	elseif __FORM.suboption == "cert_setting" then
		return ssl.form_entity_settings(form,name)
	elseif __FORM.suboption == "srv_cert" then
		return form_serverCert(form,name)
	else
		return form_serverSetting(form,name)
	end
end

function form_serverSetting(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_server_type#Server Mode").." - "..name)
  else
    form:Add("subtitle",tr("openvpn_server_type#Server Mode").." - "..name)
  end
	form:Add("select","openvpn."..name..".enable",uci.check_set("openvpn",name,"enable","1"),tr("openvpn_service#Service"),"string")
	form["openvpn."..name..".enable"].options:Add("0","Disable")
	form["openvpn."..name..".enable"].options:Add("1","Enable")
  form:Add_help(tr("openvpn_var_Service#Service"),tr("openvpn_help_Service#Enable or disable service."))

	form:Add("select","openvpn."..name..".tls_server",uci.get("openvpn",name,"tls_server"),tr("openvpn_service#TLS-SERVER"),"string")
	form["openvpn."..name..".tls_server"].options:Add("","Disable")
	form["openvpn."..name..".tls_server"].options:Add("1","Enable")
	form:Add("text", "openvpn."..name..".local", uci.get("openvpn",name,"local"),tr("openvpn_var_local#Local IP"),"string")

	form:Add("select","openvpn."..name..".proto",uci.check_set("openvpn",name,"proto","udp"),tr("openvpn_var_proto#Protocol"),"string")
	form["openvpn."..name..".proto"].options:Add("udp","udp")
	form["openvpn."..name..".proto"].options:Add("tcp","tcp")
  form:Add_help(tr("openvpn_var_proto#Protocol"),tr("openvpn_help_proto#Are we connecting to a TCP or UDP server?  Use the same setting as on the server."))
	form:Add("text", "openvpn."..name..".port", uci.check_set("openvpn",name,"port","1194"),tr("openvpn_var_port#Local Port"),"string")

	form:Add("select","openvpn."..name..".dev",uci.get("openvpn",name,"dev"),tr("openvpn_var_dev#Network"),"string")
	form["openvpn."..name..".dev"].options:Add("tun","Routed (TUN)")
	form["openvpn."..name..".dev"].options:Add("tap","Bridged (TAP)")
  form:Add_help(tr("openvpn_var_dev#Device"),tr("openvpn_help_dev#<strong>dev tun</strong> will create a routed IP tunnel<br><strong>dev tap</strong> will create an ethernet tunnel<br>Use <strong>dev tap0</strong> if you are ethernet bridging and have precreated a tap0 virtual interface and bridged it with your ethernet interface. If you want to control access policies over the VPN, you must create firewall rules for the the TUN/TAP interface. On non-Windows systems, you can give an explicit unit number, such as tun0. On Windows, use <strong>dev-node</strong> for this. On most systems, the VPN will not function unless you partially or fully disable the firewall for the TUN/TAP interface."))

	form:Add("text", "openvpn."..name..".server", uci.get("openvpn",name,"server"),tr("openvpn_var_server#Server VPN Subnet"),"string","width:100%")
  form:Add_help(tr("openvpn_var_server#Server VPN Subnet"),tr("openvpn_help_server#Configure server mode and supply a VPN subnet for OpenVPN to draw client addresses from. The server will take 10.8.0.1 for itself, the rest will be made available to clients. Each client will be able to reach the server on 10.8.0.1. Comment this line out if you are ethernet bridging. See the man page for more info."))

	form:Add("text", "openvpn."..name..".ifconfig_pool_persist", uci.get("openvpn",name,"ifconfig_pool_persist"),tr("openvpn_var_ifconfig_pool_persist#Persist IP"),"string","width:100%")
  form:Add_help(tr("openvpn_var_ifconfig_pool_persist#Persist IP"),tr("openvpn_help_ifconfig_pool_persist#Maintain a record of client <-> virtual IP address associations in this file.  If OpenVPN goes down or is restarted, reconnecting clients can be assigned the same virtual IP address from the pool that was previously assigned."))

	form:Add("text", "openvpn."..name..".server_bridge", uci.get("openvpn",name,"server_bridge"),tr("openvpn_var_server_bridge#Server Bridge"),"string","width:100%")
  form:Add_help(tr("openvpn_var_server_bridge#Server Bridge"),tr("openvpn_help_server_bridge#Configure server mode for ethernet bridging. You must first use your OS's bridging capability to bridge the TAP interface with the ethernet NIC interface.  Then you must manually set the IP/netmask on the bridge interface, here we assume 10.8.0.4/255.255.255.0.  Finally we must set aside an IP range in this subnet (start=10.8.0.50 end=10.8.0.100) to allocate to connecting clients.  Leave this line commented out unless you are ethernet bridging."))

	form:Add("list_add", "uci_list:openvpn."..name..".route", uci.get("openvpn", name, "route"),tr("openvpn_var_route#Route List"),"","width:100%")
  form:Add_help(tr("openvpn_var_route#Route List"),tr("openvpn_help_route#Push routes to the client to allow it to reach other private subnets behind the server.  Remember that these private subnets will also need to know to route the OpenVPN client address pool (10.8.0.0/255.255.255.0) back to the OpenVPN server."))

	form:Add("text", "openvpn."..name..".client_config_dir", uci.get("openvpn",name,"client_config_dir"),tr("openvpn_var_client_config_dir#Client Config Dir"),"string","width:100%")
	form:Add("text", "openvpn."..name..".learn_address", uci.get("openvpn",name,"learn_address"),tr("openvpn_var_learn_address#Script Learn Address"),"string","width:100%")
  form:Add_help(tr("openvpn_var_client_config_dir#Client Config Dir"),tr([[openvpn_help_client_config_dir#
	To assign specific IP addresses to specific
	clients or if a connecting client has a private
	subnet behind it that should also have VPN access,
	use the subdirectory "ccd" for client-specific
	configuration files (see man page for more info).<br><br>
	EXAMPLE: Suppose the client
	having the certificate common name "Thelonious"
	also has a small subnet behind his connecting
	machine, such as 192.168.40.128/255.255.255.248.<br>
	First, uncomment out these lines:<br>
	&nbsp;&nbsp;&nbsp;&nbsp;option client_config_dir /etc/openvpn/ccd<br>
	&nbsp;&nbsp;&nbsp;&nbsp;list route "192.168.40.128 255.255.255.248"<br>
	Then create a file ccd/Thelonious with this line:<br>
	&nbsp;&nbsp;&nbsp;&nbsp;iroute 192.168.40.128 255.255.255.248<br>
	This will allow Thelonious' private subnet to
	access the VPN.  This example will only work
	if you are routing, not bridging, i.e. you are
	using "dev tun" and "server" directives.<br><br>
	EXAMPLE: Suppose you want to give
	Thelonious a fixed VPN IP address of 10.9.0.1.<br>
	First uncomment out these lines:<br>
	&nbsp;&nbsp;&nbsp;&nbsp;option client_config_dir /etc/openvpn/ccd<br>
	&nbsp;&nbsp;&nbsp;&nbsp;list route "10.9.0.0 255.255.255.252"<br>
	&nbsp;&nbsp;&nbsp;&nbsp;list route "192.168.100.0 255.255.255.0"<br>
	Then add this line to ccd/Thelonious:<br>
	&nbsp;&nbsp;&nbsp;&nbsp;ifconfig-push "10.9.0.1 10.9.0.2"<br><br>
	Suppose that you want to enable different
	firewall access policies for different groups
	of clients.<br>
	There are two methods:<br><br>
	(1) Run multiple OpenVPN daemons, one for each<br>
	&nbsp;&nbsp;&nbsp;&nbsp;group, and firewall the TUN/TAP interface<br>
	&nbsp;&nbsp;&nbsp;&nbsp;for each group/daemon appropriately.<br><br>
	(2) (Advanced) Create a script to dynamically<br>
	&nbsp;&nbsp;&nbsp;&nbsp;modify the firewall in response to access<br>
	&nbsp;&nbsp;&nbsp;&nbsp;from different clients.<br><br>
	See man page for more info on learn-address script.
]]))
--[[
	local tpush = uci.get("openvpn",name,"push")
	local str_push = ""
	if tpush then
		for k, v in ipairs(tpush) do
			str_push = str_push .. " " .. v
		end
	end
]]
	form:Add("list_add", "uci_list:openvpn."..name..".push", uci.get("openvpn",name,"push"),tr("openvpn_var_push#Push List"),"","width:100%")
  form:Add_help(tr("openvpn_var_push#Push List"),tr([[openvpn_help_push#
	If enabled, this directive will configure
	all clients to redirect their default
	network gateway through the VPN, causing
	all IP traffic such as web browsing and
	and DNS lookups to go through the VPN
	(The OpenVPN server machine may need to NAT
	the TUN/TAP interface to the internet in
	order for this to work properly).<br>
	CAVEAT: May break client's network config if
	client's local DHCP server packets get routed
	through the tunnel.  Solution: make sure
	client's local DHCP server is reachable via
	a more specific route than the default route
	of 0.0.0.0/0.0.0.0.<br>
	push "redirect-gateway"<br><br>
	Certain Windows-specific network settings
	can be pushed to clients, such as DNS
	or WINS server addresses.<br>
	CAVEAT:	http://openvpn.net/faq.html%23dhcpcaveats<br>
	push "dhcp-option DNS 10.8.0.1"<br>
	push "dhcp-option WINS 10.8.0.1"<br>
	]]))

	form:Add("text", "openvpn."..name..".client_to_client", uci.get("openvpn",name,"client_to_client"),tr("openvpn_var_client_to_client#Client to Client"),"string","width:100%")
  form:Add_help(tr("openvpn_var_client_to_client#Client to Client"),tr([[openvpn_help_client_to_client#
	This directive to allow different
	clients to be able to "see" each other.
	By default, clients will only see the server.
	To force clients to only see the server, you
	will also need to appropriately firewall the
	server's TUN/TAP interface.]]))

	form:Add("text", "openvpn."..name..".duplicate_cn", uci.get("openvpn",name,"duplicate_cn"),tr("openvpn_var_duplicate_cn#Duplicate CN"),"string","width:100%")
  form:Add_help(tr("openvpn_var_duplicate_cn#Duplicate CN"),tr([[openvpn_help_duplicate_cn#
	This directive if multiple clients
	might connect with the same certificate/key
	files or common names.  This is recommended
	only for testing purposes.  For production use,
	each client should have its own certificate/key
	pair.<br><br>
	IF YOU HAVE NOT GENERATED INDIVIDUAL
	CERTIFICATE/KEY PAIRS FOR EACH CLIENT,
	EACH HAVING ITS OWN UNIQUE "COMMON NAME",
	UNCOMMENT THIS LINE OUT.
	]]))

	form:Add("text", "openvpn."..name..".keepalive", uci.get("openvpn",name,"keepalive"),tr("openvpn_var_keepalive#Keep Alive"),"string")
  form:Add_help(tr("openvpn_var_keepalive#Keep Alive"),tr([[openvpn_help_keepalive#
	Keepalive directive causes ping-like
	messages to be sent back and forth over
	the link so that each side knows when
	the other side has gone down.
	Ping every 10 seconds, assume that remote
	peer is down if no ping received during
	a 120 second time period.	]]))

	form:Add("text", "openvpn."..name..".cipher", uci.get("openvpn",name,"cipher"),tr("openvpn_var_cipher#Cryptographic Cipher"),"string")
  form:Add_help(tr("openvpn_var_cipher#Cryptographic Cipher"),tr([[openvpn_help_cipher#
		Select a cryptographic cipher.
		This config item must be copied to
		the client config file as well.<br>
		Blowfish (default): BF-CBC<br>
		AES: AES-128-CBC<br>
		Triple-DES: DES-EDE3-CBC
	]]))

	form:Add("select","openvpn."..name..".comp_lzo",uci.check_set("openvpn",name,"comp_lzo","1"),tr("openvpn_var_comp_lzo#Compression"),"string")
	form["openvpn."..name..".comp_lzo"].options:Add("0","Disable")
	form["openvpn."..name..".comp_lzo"].options:Add("1","Enable")
  form:Add_help(tr("openvpn_var_comp_lzo#Compression"),tr("openvpn_help_comp_lzo#Enable compression on the VPN link. Don't enable this unless it is also enabled in the client config file."))
	form:Add("text", "openvpn."..name..".max_clients", uci.get("openvpn",name,"max_clients"),tr("openvpn_var_max_clients#Max Client Connections"),"string")
  form:Add_help(tr("openvpn_var_max_clients#Max Client Connections"),tr([[openvpn_help_max_clients#
	The maximum number of concurrently connected clients we want to allow.
	]]))

	form:Add("select","openvpn."..name..".persist_key",uci.check_set("openvpn",name,"persist_key","1"),tr("openvpn_var_persist_key#Persist Key"),"string")
	form["openvpn."..name..".persist_key"].options:Add("0",tr("No"))
	form["openvpn."..name..".persist_key"].options:Add("1",tr("Yes"))
	form:Add("select","openvpn."..name..".persist_tun",uci.check_set("openvpn",name,"persist_tun","1"),tr("openvpn_var_persist_tun#Persist Tunnel"),"string")
	form["openvpn."..name..".persist_tun"].options:Add("0",tr("No"))
	form["openvpn."..name..".persist_tun"].options:Add("1",tr("Yes"))
  form:Add_help(tr("openvpn_persist#Persist"),tr([[openvpn_help_server_persist#
	The persist options will try to avoid
	accessing certain resources on restart
	that may no longer be accessible because
	of the privilege downgrade.
	]]))
	form:Add("text", "openvpn."..name..".status", uci.get("openvpn",name,"status"),tr("openvpn_var_status#Status File"),"string","width:100%")
  form:Add_help(tr("openvpn_var_status#Status File"),tr([[openvpn_help_status#
	Output a short status file showing current connections, truncated and rewritten every minute.<br>
	Example status: /tmp/openvpn-status.log
	]]))

	form:Add("select","openvpn."..name..".verb",uci.check_set("openvpn",name,"verb","3"),tr("openvpn_var_verb#Log Verbosity"),"string")
	form["openvpn."..name..".verb"].options:Add("0","0")
	form["openvpn."..name..".verb"].options:Add("1","1")
	form["openvpn."..name..".verb"].options:Add("2","2")
	form["openvpn."..name..".verb"].options:Add("3","3")
	form["openvpn."..name..".verb"].options:Add("4","4")
	form["openvpn."..name..".verb"].options:Add("5","5")
	form["openvpn."..name..".verb"].options:Add("6","6")
	form["openvpn."..name..".verb"].options:Add("7","7")
	form["openvpn."..name..".verb"].options:Add("8","8")
	form["openvpn."..name..".verb"].options:Add("9","9")
  form:Add_help(tr("openvpn_var_verb#Log Verbosity"),tr([[openvpn_help_verb#
	Set the appropriate level of log file verbosity.
	<br><br>
	0 is silent, except for fatal errors<br>
	4 is reasonable for general usage<br>
	5 and 6 can help to debug connection problems<br>
	9 is extremely verbose
	]]))
	form:Add("text", "openvpn."..name..".mute", uci.get("openvpn",name,"mute"),tr("openvpn_var_mute#Silence repeating messages"),"string")
	forms = {}
	forms[#forms+1] = form
	return forms
end

function form_serverCert(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_server_type#Server Certificates").." - /etc/openssl/"..name,true)
  else
    form:Add("subtitle",tr("openvpn_server_type#Server Certificates").." - /etc/openssl/"..name)
  end
	if io.exists("/etc/openssl/"..name.."/cacert.crt") then
		if (uci.get("openvpn",name,"ca") == nil) 
		and uci.get("openvpn",name,"mode") then
			uci.set("openvpn",name,"ca","/etc/openssl/"..name.."/cacert.crt")
			uci.save("openvpn")
			uci.commit("openvpn")
		end
	else
		if uci.get("openvpn",name,"ca") then
			uci.delete("openvpn",name,"ca")
			uci.save("openvpn")
			uci.commit("openvpn")
		end
	end

	local strh = ""
	strh = strh .. "<table width='100%' style='font-size:80%;'>\n"
	strh = strh .. "\t<tr style='background: #c8c8c8;'>"
	strh = strh ..[[<th style="width:95px;">]].."Certificate"..[[</th>]]
	strh = strh ..[[<th>]].."File"..[[</th>]]
--	strh = strh ..[[<th>]].."file"..[[</th>]]
	strh = strh ..[[<th colspan="4" style="text-align: center;" >]].."Action"..[[</th>]]
	strh = strh .. "\t</tr>\n"

	local str = strh
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."CA CRT"..[[</th>]]
--	str = str ..[[<td>]].."/etc/openssl/"..name..[[</td>]]
--	str = str ..[[<td>]]..(uci.get("openvpn",name,"ca") or "")..[[</td>]]
	local srvfile = check_srvFile(name,"/cacert.crt")
	
	str = str ..[[<td>]]..srvfile..[[</td>]]
--	if uci.get("openvpn",name,"ca") == nil then
	if srvfile == "" then
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:]]..name..[[:new:cacert.crt" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	else
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[/cacert.crt:info" value="Info" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:cacert.crt]]..[[:download:]]..name..[[:cacert.crt" value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:ca:del:]]..name..[[:cacert.crt" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	end
	str = str ..[[</tr>]].."\n"
	str = str ..[[</table>]]

	if srvfile ~= "" then
	str = str .. strh
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."Server CRT"..[[</th>]]
--	str = str ..[[<td>]].."/etc/openssl/"..name..[[/certs</td>]]
	srvfile = check_srvFile(name,"/certs/server.crt")
	if srvfile == "" then
		uci.delete("openvpn",name,"cert")
	else
		if uci.get("openvpn",name,"mode") then
			uci.check_set("openvpn",name,"cert",srvfile)
		end
	end
	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str ..[[<td>]].."server.crt"..[[</td>]]
	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:certs:server" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[/certs/server.crt:info" value="Info" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[:revoke:server:server" value="Revoke" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[:del:server:server" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str ..[[</tr>]].."\n"
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."Server KEY"..[[</th>]]
--	str = str ..[[<td>]].."/etc/openssl/"..name..[[/certs</td>]]
--	str = str ..[[<td>]]..(uci.get("openvpn",name,"key") or "")..[[</td>]]
	srvfile = check_srvFile(name,"/private/server.key")
	if srvfile == "" then
		uci.delete("openvpn",name,"key")
	else
		if uci.get("openvpn",name,"mode") then
			uci.check_set("openvpn",name,"key",srvfile)
		end
	end
	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str ..[[<td>]].."server.key"..[[</td>]]
	str = str ..[[</tr>]].."\n"
	str = str .. "\t<tr >"
	str = str ..[[</table>]]
	end
	
	str = str .. strh
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."TLS Auth"..[[</th>]]
--	str = str ..[[<td>]].."/etc/openssl/"..name..[[</td>]]
--	str = str ..[[<td>]]..(uci.get("openvpn",name,"tls_auth") or "")..[[</td>]]
	srvfile = check_srvFile(name,"/ta.key")
	if srvfile == "" then
		uci.delete("openvpn",name,"tls_auth")
	else
		if uci.get("openvpn",name,"mode") then
			uci.check_set("openvpn",name,"tls_auth",srvfile)
		end
	end
	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str ..[[<td>]].."psk.pem"..[[</td>]]
--	str = str .."\t\t"..[[<td style="width:80px">&nbsp;</td>]]
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:]].."ta.key"..[[" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:ta.key]]..[[:download:]]..name..[[:ta.key"  value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:del:]].."ta.key"..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str ..[[</tr>]].."\n"
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."Diffie Hellman"..[[</th>]]
--	str = str ..[[<td>]].."/etc/openssl/"..name..[[</td>]]
--	str = str ..[[<td>]]..(uci.get("openvpn",name,"dh") or "")..[[</td>]]
	srvfile = check_srvFile(name,"/dh.pem")
	if srvfile == "" then
		uci.delete("openvpn",name,"dh")
	else
		if uci.get("openvpn",name,"mode") then
			uci.check_set("openvpn",name,"dh",srvfile)
		end
	end
	uci.save("openvpn")
	uci.commit("openvpn")

	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str ..[[<td>]].."dh.pem"..[[</td>]]
--	str = str .."\t\t"..[[<td style="width:80px">&nbsp;</td>]]
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:]].."dh.pem"..[[" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:dh.pem]]..[[:download:]]..name..[[:dh.pem" value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:del:]].."dh.pem"..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str ..[[</tr>]].."\n"
	str = str ..[[</table>]]
	form:Add("text_line","line1",str)
--[[
--	form:Add("uci_file", "openvpn."..name..".ca", uci.get("openvpn",name,"ca"),tr("openvpn_var_srv_ca#CA File"),"","width:100%")
	form:Add("subtitle", "Server Certificates")
	form:Add("uci_file", "openvpn."..name..".cert", uci.get("openvpn",name,"cert"),tr("openvpn_var_srv_cert#Certificate File"),"","width:100%")
	form:Add("uci_file", "openvpn."..name..".key", uci.get("openvpn",name,"key"),tr("openvpn_var_srv_key#Key File"),"","width:100%")
	form:Add("subtitle", "SSL/TLS")
	form:Add("uci_file", "openvpn."..name..".tls_auth", uci.get("openvpn",name,"tls_auth"),tr("openvpn_var_tls_auth#TLS Auth File"),"","width:100%")
--	form:Add_help(tr("openvpn_var_SSLTLSparms#SSL/TLS parms (cert/key files)"),tr("openvpn_help_SSLTLSparms#SSL/TLS root certificate (ca), certificate (cert), and private key (key).  Each client and the server must have their own cert and key file.  The server and all clients will use the same ca file.<br>See the <strong>easy-rsa</strong> directory for a series of scripts for generating RSA certificates and private keys.  Remember to use a unique Common Name for the server and each of the client certificates.<br>Any X509 key management system can be used. OpenVPN can also use a PKCS #12 formatted key file (see <strong>pkcs12</strong> directive in man page)."))
	form:Add("subtitle", "Diffie hellman parameters")
	form:Add("uci_file", "openvpn."..name..".dh", uci.get("openvpn",name,"dh"),tr("openvpn_var_dh#Diffie Hellman"),"","width:100%")
]]
--  form:Add_help(tr("openvpn_var_dh#Diffie hellman parameters"),tr("openvpn_help_dh#Generate your own with:<br>openssl dhparam -out dh1024.pem 1024 <br>Substitute 2048 for 1024 if you are using 2048 bit keys."))
--  form:Add_help(tr("openvpn_var_tls_auth#TLS Auth File"),tr([[openvpn_help_tls_auth#
--	For extra security beyond that provided
--	by SSL/TLS, create an "HMAC firewall"
--	to help block DoS attacks and UDP port flooding.<br>
--	<br>
--	Generate with:<br>
--	&nbsp;&nbsp;&nbsp;&nbsp;openvpn --genkey --secret ta.key<br>
--	<br>
--	The server and each client must have
--	a copy of this key.<br>
--	The second parameter should be '0'
--	on the server and '1' on the clients.<br>
--	This file is secret: "/etc/openvpn/ta.key 0"
--	]]))
	forms = {}
	forms[#forms+1] = form
	return forms
end

function check_srvFile(name,file)
	local srvfile = ""
--	if srvfile == nil then
		if io.exists("/etc/openssl/"..name..file) == true then
			srvfile = "/etc/openssl/"..name..file
		end
--	end
	return srvfile
end

--[[
function form_setcertificates(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_set_certificates#Certificate's Settings").." - "..name)
  else
    form:Add("subtitle",tr("openvpn_set_certificates#Certificate's Settings").." - "..name)
  end
	form:Add("text","openvpn."..name.."key_org", uci.get("openvpn",name,"key_org"),tr("openvpn_key_org#Organization Name"),"string")
	form:Add("text","openvpn."..name.."key_org_unit", uci.get("openvpn",name,"key_org_unit"),tr("openvpn_key_org_unit#Organization Unit Name"),"string")
	form:Add("text","openvpn."..name.."key_email", uci.get("openvpn",name,"key_email"),tr("openvpn_key_email#Email Address"),"string")

	form:Add("text","openvpn."..name.."key_city", uci.get("openvpn",name,"key_city"),tr("openvpn_key_city#Locality Name (city, district)"),"string")
	form:Add("text","openvpn."..name.."key_province", uci.get("openvpn",name,"key_province"),tr("openvpn_key_province#State or Province Name"),"string")
	form:Add("text","openvpn."..name.."key_country", uci.check_set("openvpn",name,"key_country","US"),tr("openvpn_key_country#Country Name (2 letters code name)"),"string")
	form:Add("text","openvpn."..name.."key_common_name", uci.get("openvpn",name,"key_common_name"),tr("openvpn_key_common_name#Common Name (hostname, IP or your name)"),"string")
			
	form:Add("text","openvpn."..name.."key_size", uci.check_set("openvpn",name,"key_size","1024"),tr("openvpn_key_size#Key Size"),"string")
	form:Add("text","openvpn."..name.."key_expire", uci.check_set("openvpn",name,"key_expire","3650"),tr("openvpn_key_expire#Key Expire"),"string")
	forms = {}
	forms[#forms+1] = form
	return forms

end
]]

--function form_srvcertificates(form,name)
--	local forms = {}
--  if form == nil then
--    form = formClass.new(tr("openvpn_srv_certificates#Server Certificates").." - "..name,true)
--  else
--    form:Add("subtitle",tr("openvpn_srv_certificates#Server Certificates").." - "..name)
--  end
--	local crtsList = get_srv_certs(name)
--	local str = [[<div class="settings-content">]]
--	str = str ..[[<table width="100%">]]
--	str = str .."<tr>"
--	str = str .. "<td width=\"40%\">" .. tr("openvpn_new_clicert#Common Name") .. "</td>"
--	str = str .."<td width=\"60%\">"
--	str = str ..		"<table width='100%'>"
--	str = str ..		"<tr>"
--	str = str ..  		[[<td><input type="text" name="new_cert_name" value="" /></td>]]
--	str = str ..  		[[<td><input type="submit" name="cert:client:new:]]..name..[[" value="]]..tr("openvpn_createNewCert#New Certificate")..[[" /></td>]]
--	str = str ..		"</tr>"
--	str = str .."		</table>"
--	str = str .. "</td>"
--	str = str .. "</tr>"
--	str = str ..[[</table>]]
--	str = str ..[[</div>]]
--	str = str .. "<table width='100%' style='font-size:80%;'>\n"
--	str = str .. "\t<tr style='background: #c8c8c8;'>"
--	str = str ..[[<th style="width:25px">]].."Idx"..[[</th>]]
--	str = str ..[[<th>]].."Name"..[[</th>]]
--	str = str ..[[<th colspan="2">]].."Download"..[[</th>]]
--	str = str ..[[<th style="width:180px">]].."Not Before"..[[</th>]]
--	str = str ..[[<th style="width:180px">]].."Not After"..[[</th>]]
--	str = str ..[[<th style="width:80px">]].."Action"..[[</th>]]
--	str = str ..[[</tr>]].."\n"
--	for f, data in util.pairsByKeys(crtsList) do
--		str = str .. "\t<tr>"
--		if data.idx then
--			str = str ..[[<td align="right">]]..data.idx..[[</td>]]
--		else
--			str = str ..[[<td align="right">&nbsp;</td>]]
--		end
--		str = str ..[[<td>]]..f..[[</td>]]
--		str = str ..[[<td style="width:35px;"><a href="/openvpn/]]..name..[[/]]..f..[[.crt">crt</a></td>]]
--		str = str ..[[<td style="width:35px;"><a href="/openvpn/]]..name..[[/]]..f..[[.key">key</a></td>]]
--		if data["Not Before"] then
--			str = str ..[[<td>]]..data["Not Before"]..[[</td>]]
--			str = str ..[[<td>]]..data["Not After"]..[[</td>]]
--		else
--			str = str ..[[<td align="right">&nbsp;</td>]]
--			str = str ..[[<td align="right">&nbsp;</td>]]
--		end
--		str = str ..[[<td style="width:80px">]]..[[<input type="submit" name="cert:client:del:]]..name..":"..f..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]]
--		str = str ..[[</tr>]].."\n"
--	end
--	str = str .."</table>\n"
--	form:Add("text_line","linea1",str)
--	
--	forms[#forms+1] = form
--	return forms
--
--end

--[[
function get_client_certs(name)
	local crtList = io.popen("ls /www/openvpn/"..name.."/clients")
	local crtTable = {}
	for line in crtList:lines() do
		local crtname = string.gsub(line,"(.+)%.%w+$","%1")
		if crtTable[crtname] == nil then
			crtTable[crtname] = {}
		end
	end
	return certData(crtTable,name)
end
]]

--[[
function get_srv_certs(name)
	local crtList = io.popen("ls /www/openvpn/"..name)
	local crtTable = {}
	for line in crtList:lines() do
		if line ~= "clients" then
			local crtname = string.gsub(line,"(.+)%.%w+$","%1")
			if crtTable[crtname] == nil then
				crtTable[crtname] = {}
			end
		end
	end
	return certData(crtTable,name)
end
]]
--[[
function certData(crtTable,name)
	local data = util.file_load("/etc/openvpn/"..name.."/certindex.txt")
	for line in string.gmatch(data,"[^\n]+") do
		local _, _, idx, fname = string.find(line,".+%s+(%d%d)%s+.+CN=(.+)")
		dates = util.file_load("/etc/openvpn/"..name.."/certs/"..idx..".pem")
		local _, _, sini, send = string.find(dates,"Validity\n(.+)\n%s+Subject:.+")
		if crtTable[fname] then
			crtTable[fname].idx = idx
			for d in string.gmatch(sini,"[^\n]+") do
				local _, _, key, value = string.find(d, "%s+(.+):%s+(.*)")
				if key then
					key = string.trim(key)
					crtTable[fname][key]=value
				end
			end
		end
	end
	return crtTable
end
]]

--function form_clicertificates(form,name)
--	local form = form
--	forms = {}
--  if form == nil then
--    forms[#forms+1] = formClass.new(tr("openvpn_cli_certificates#Certificates for Clients").." - "..name,true)
--  else
--    forms[#forms+1]:Add("subtitle",tr("openvpn_cli_certificates#Certificates for Clients").." - "..name)
--  end
--	form = forms[#forms]
	
--	local crtsList = get_client_certs(name)
--	local str = [[<div class="settings-content">]]
--	str = str ..[[<table width="100%">]]
--	str = str .."<tr>"
--	str = str .. "<td width=\"40%\">" .. tr("openvpn_new_clicert#Common Name") .. "</td>"
--	str = str .."<td width=\"60%\">"
--	str = str ..		"<table width='100%'>"
--	str = str ..		"<tr>"
--	str = str ..  		[[<td><input type="text" name="new_cert_name" value="" /></td>]]
--	str = str ..  		[[<td><input type="submit" name="cert:client:new:]]..name..[[" value="]]..tr("openvpn_createNewCert#New Certificate")..[[" /></td>]]
--	str = str ..		"</tr>"
--	str = str .."		</table>"
--	str = str .. "</td>"
--	str = str .. "</tr>"
--	str = str ..[[</table>]]
--	str = str ..[[</div>]]
--	str = str .. "<table width='100%' style='font-size:80%;'>\n"
--	str = str .. "\t<tr style='background: #c8c8c8;'>"
--	str = str ..[[<th style="width:25px">]].."Idx"..[[</th>]]
--	str = str ..[[<th>]].."Name"..[[</th>]]
--	str = str ..[[<th colspan="2">]].."Download"..[[</th>]]
--	str = str ..[[<th style="width:180px">]].."Not Before"..[[</th>]]
--	str = str ..[[<th style="width:180px">]].."Not After"..[[</th>]]
--	str = str ..[[<th style="width:80px">]].."Action"..[[</th>]]
--	str = str ..[[</tr>]].."\n"
--	for f, data in util.pairsByKeys(crtsList) do
--		str = str .. "\t<tr>"
--		if data.idx then
--			str = str ..[[<td align="right">]]..data.idx..[[</td>]]
--		else
--			str = str ..[[<td align="right">&nbsp;</td>]]
--		end
--		str = str ..[[<td>]]..f..[[</td>]]
--		str = str ..[[<td style="width:35px;"><a href="/openvpn/]]..name..[[/clients/]]..f..[[.crt">crt</a></td>]]
--		str = str ..[[<td style="width:35px;"><a href="/openvpn/]]..name..[[/clients/]]..f..[[.key">key</a></td>]]
--		if data["Not Before"] then
--			str = str ..[[<td>]]..data["Not Before"]..[[</td>]]
--			str = str ..[[<td>]]..data["Not After"]..[[</td>]]
--		else
--			str = str ..[[<td align="right">&nbsp;</td>]]
--			str = str ..[[<td align="right">&nbsp;</td>]]
--		end
--		str = str ..[[<td style="width:80px">]]..[[<input type="submit" name="cert:client:del:]]..name..":"..f..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]]
--		str = str ..[[</tr>]].."\n"
--	end
--	str = str .."</table>\n"
--	form:Add("text_line","linea1",str)
--	return forms
--end

function form_client(form,name)
	if __FORM.suboption == "cli_certificates" then
		return form_clientCert(form,name)
	else
		return form_clientSetting(form, name)
	end
end

function form_clientSetting(form,name)
	local mode = "Client"
	if uci.get("openvpn."..name..".tls_client") then
		mode = "TLS-Client "
	end
  if form == nil then
    form = formClass.new(mode.." Mode".." - "..name)
  else
    form:Add("subtitle",mode.." Mode".." - "..name)
  end
	form:Add("select","openvpn."..name..".enable",uci.check_set("openvpn",name,"enable","1"),tr("openvpn_service#Service"),"string")
	form["openvpn."..name..".enable"].options:Add("0","Disable")
	form["openvpn."..name..".enable"].options:Add("1","Enable")
	form:Add("select","openvpn."..name..".tls_client",uci.check_set("openvpn",name,"tls_client","1"),tr("openvpn_var_tls_client#TLS Client"),"string")
	form["openvpn."..name..".tls_client"].options:Add("0","Disable")
	form["openvpn."..name..".tls_client"].options:Add("1","Enable")
	form:Add("select","openvpn."..name..".script_security",uci.check_set("openvpn",name,"script_security","2"),tr("openvpn_service#Script Security"),"string")
	form["openvpn."..name..".script_security"].options:Add("1","1")
	form["openvpn."..name..".script_security"].options:Add("2","2")
--  form:Add_help(tr("openvpn_var_Service#Service"),tr("openvpn_help_Service#Enable or disable service."))
	form:Add("select","openvpn."..name..".dev",uci.check_set("openvpn",name,"dev","tun"),tr("openvpn_var_dev#Network"),"string")
	form["openvpn."..name..".dev"].options:Add("tun","Routed (TUN)")
	form["openvpn."..name..".dev"].options:Add("tap","Bridged (TAP)")
  form:Add_help(tr("openvpn_var_dev#Network"),tr("openvpn_help_dev#Use the same setting as you are using on the server. On most systems, the VPN will not function unless you partially or fully disable the firewall for the TUN/TAP interface."))
	form:Add("select","openvpn."..name..".proto",uci.check_set("openvpn",name,"proto","udp"),tr("openvpn_var_proto#Protocol"),"string")
	form["openvpn."..name..".proto"].options:Add("udp","udp")
	form["openvpn."..name..".proto"].options:Add("tcp","tcp")
	form:Add("text", "openvpn."..name..".port", uci.check_set("openvpn",name,"port","1194"),tr("openvpn_var_porty#Local Port"),"string")
  form:Add_help(tr("openvpn_var_proto#Protocol"),tr("openvpn_help_proto#Are we connecting to a TCP or UDP server?  Use the same setting as on the server."))
	form:Add("list_add", "uci_list:openvpn."..name..".remote", uci.get("openvpn",name,"remote"), tr("openvpn_var_remote#Server List"),"","width:100%")
  form:Add_help(tr("openvpn_var_remote#Servers List"),tr("openvpn_help_remote#The hostname/IP and port of the server. You can have multiple remote entries to load balance between the servers."))
	form:Add("select","openvpn."..name..".remote_random",uci.get("openvpn",name,"remote_random"),tr("openvpn_var_remote_random#Remote Random"),"string")
	form["openvpn."..name..".remote_random"].options:Add("","")
	form["openvpn."..name..".remote_random"].options:Add("0",tr("No#No"))
	form["openvpn."..name..".remote_random"].options:Add("1",tr("Yes#Yes"))
  form:Add_help(tr("openvpn_var_remote_random#Remote Random"),tr("openvpn_help_remote_random#Choose a random host from the remote list for load_balancing.  Otherwise try hosts in the order specified."))
	form:Add("text", "openvpn."..name..".resolv_retry", uci.get("openvpn",name,"resolv_retry"),tr("openvpn_var_resolv_retry#Resolve Retry"),"string")
--	form:Add("select","openvpn."..name..".resolv_retry",uci.check_set("openvpn",name,"resolv_retry","1"),tr("openvpn_var_resolv_retry#Resolve Retry"),"string")
--	form["openvpn."..name..".resolv_retry"].options:Add("infinite",tr("Infinite#Infinite"))
--	form["openvpn."..name..".resolv_retry"].options:Add("otro",tr("Otro#Otro"))
  form:Add_help(tr("openvpn_var_resolv_retry#Resolve Retry"),tr("openvpn_help_resolv_retry#Keep trying indefinitely to resolve the host name of the OpenVPN server.  Very useful on machines which are not permanently connected to the internet such as laptops."))
	form:Add("text", "openvpn."..name..".nobind", uci.get("openvpn",name,"nobind"),tr("openvpn_var_nobind#No Bind"),"string")
  form:Add_help(tr("openvpn_var_nobind#No Bind"),tr("openvpn_help_nobind#Most clients don't need to bind to a specific local port number."))
	form:Add("select","openvpn."..name..".persist_key",uci.check_set("openvpn",name,"persist_key","1"),tr("openvpn_var_persist_key#Persist Key"),"string")
	form["openvpn."..name..".persist_key"].options:Add("0",tr("No"))
	form["openvpn."..name..".persist_key"].options:Add("1",tr("Yes"))
	form:Add("select","openvpn."..name..".persist_tun",uci.check_set("openvpn",name,"persist_tun","1"),tr("openvpn_var_persist_tun#Persist Tunnel"),"string")
	form["openvpn."..name..".persist_tun"].options:Add("0",tr("No"))
	form["openvpn."..name..".persist_tun"].options:Add("1",tr("Yes"))
  form:Add_help(tr("openvpn_persist#Persist"),tr("openvpn_help_persist#Try to preserve some state across restarts."))
	form:Add("select","openvpn."..name..".float",uci.get("openvpn",name,"float"),tr("openvpn_var_float#Float"),"string")
	form["openvpn."..name..".float"].options:Add("","No")
	form["openvpn."..name..".float"].options:Add("1",tr("Yes"))
	form:Add("select","openvpn."..name..".auto_proxy",uci.get("openvpn",name,"auto_proxy"),tr("openvpn_var_auto_proxy#Auto Proxy"),"string")
	form["openvpn."..name..".auto_proxy"].options:Add("","")
	form["openvpn."..name..".auto_proxy"].options:Add("0",tr("No"))
	form["openvpn."..name..".auto_proxy"].options:Add("1",tr("Yes"))
	form:Add("select","openvpn."..name..".http_proxy_retry",uci.get("openvpn",name,"http_proxy_retry"),tr("openvpn_var_http_proxy_retry#http proxy retry"),"string")
	form["openvpn."..name..".http_proxy_retry"].options:Add("","")
	form["openvpn."..name..".http_proxy_retry"].options:Add("0",tr("No"))
	form["openvpn."..name..".http_proxy_retry"].options:Add("1",tr("Yes"))
  form:Add_help(tr("openvpn_var_http_proxy_retry#http Proxy Retry"),tr("openvpn_help_http_proxy_retry#If you are connecting through an HTTP proxy to reach the actual OpenVPN server, put the proxy server/IP and port number here.  See the man page if your proxy server requires authentication. retry on connection failures:"))
	form:Add("text", "openvpn."..name..".http_proxy", uci.get("openvpn",name,"http_proxy"),tr("openvpn_var_http_proxy#http Proxy"),"string")
  form:Add_help(tr("openvpn_var_http_proxy#http Proxy"),tr("openvpn_help_http_proxy#specify http proxy address and port:<br>192.168.1.100 8080"))
	form:Add("text", "openvpn."..name..".keepalive", uci.get("openvpn",name,"keepalive"),tr("openvpn_var_keepalive#Keep Alive"),"string")
	form:Add("text", "openvpn."..name..".ifconfig", uci.get("openvpn",name,"ifconfig"),tr("openvpn_var_ifconfig#IP/Mask or IP Local/Remote"),"string","width:100%;")
	form:Add("select","openvpn."..name..".ping_timer_rem",uci.get("openvpn",name,"ping_timer_rem"),tr("openvpn_var_ping_timer_rem#Ping Timer rem"),"string")
	form["openvpn."..name..".ping_timer_rem"].options:Add("1","Yes")
	form["openvpn."..name..".ping_timer_rem"].options:Add("0",tr("No"))
	form:Add("select","openvpn."..name..".up_restart",uci.get("openvpn",name,"up_restart"),tr("openvpn_var_up_restart#Up Restart"),"string")
	form["openvpn."..name..".up_restart"].options:Add("1","Yes")
	form["openvpn."..name..".up_restart"].options:Add("",tr("No"))

	form:Add("select","openvpn."..name..".mute_replay_warnings",uci.get("openvpn",name,"mute_replay_warnings"),tr("openvpn_var_mute_replay_warnings#Mute replay warnings"),"string")
	form["openvpn."..name..".mute_replay_warnings"].options:Add("","")
	form["openvpn."..name..".mute_replay_warnings"].options:Add("0",tr("No"))
	form["openvpn."..name..".mute_replay_warnings"].options:Add("1",tr("Yes"))
  form:Add_help(tr("openvpn_var_mute_replay_warnings#Mute replay warnings"),tr("openvpn_help_mute_replay_warnings#Wireless networks often produce a lot of duplicate packets.  Set this flag to silence duplicate packet warnings."))
	form:Add("select","openvpn."..name..".ns_cert_type",uci.check_set("openvpn",name,"ns_cert_type","server"),tr("openvpn_var_ns_cert_type#NS Cert type"),"string")
	form["openvpn."..name..".ns_cert_type"].options:Add("server",tr("Server"))
	form["openvpn."..name..".ns_cert_type"].options:Add("client",tr("Client"))
  form:Add_help(tr("openvpn_ns_cert_type#NS Cert type"),tr("openvpn_help_ns_cert_type#Verify server certificate by checking that the certicate has the nsCertType field set to <strong>server</strong>. This is an important precaution to protect against a potential attack discussed here:<br> http://openvpn.net/howto.html#mitm<br>To use this feature, you will need to generate your server certificates with the nsCertType field set to <strong>server</strong>.  The build_key_server script in the easy_rsa folder will do this."))
	form:Add("text", "openvpn."..name..".cipher", uci.get("openvpn",name,"cipher"),tr("openvpn_var_cipher#Cryptographic cipher"),"string")
--[[
	form:Add("select","openvpn."..name..".cipher",uci.get("openvpn",name,"cipher"),tr("openvpn_var_cipher#Cryptographic cipher"),"string")
	form["openvpn."..name..".cipher"].options:Add("",tr("No cipher"))
	form["openvpn."..name..".cipher"].options:Add("otro",tr("otro cipher"))
]]--
  form:Add_help(tr("openvpn_cipher#Cryptographic cipher"),tr("openvpn_help_cipher#If the cipher option is used on the server then you must also specify it here."))
	form:Add("select","openvpn."..name..".comp_lzo",uci.check_set("openvpn",name,"comp_lzo","1"),tr("openvpn_var_comp_lzo#Compression"),"string")
	form["openvpn."..name..".comp_lzo"].options:Add("0","Disable")
	form["openvpn."..name..".comp_lzo"].options:Add("1","Enable")
  form:Add_help(tr("openvpn_var_comp_lzo#Compression"),tr("openvpn_help_comp_lzo#Enable compression on the VPN link. Don't enable this unless it is also enabled in the server config file."))
	form:Add("text", "openvpn."..name..".verb", uci.get("openvpn",name,"verb"),tr("openvpn_var_verb#Log Verbosity"),"string")
	form:Add("text", "openvpn."..name..".mute", uci.get("openvpn",name,"mute"),tr("openvpn_var_mute#Silence repeating messages"),"string")
--  form:Add_help(tr("openvpn_var_mute#Service"),tr("openvpn_help_mute#Silence repeating messages."))
	forms = {}
	forms[#forms+1] = form
	return forms
end

function form_clientCert(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_client_Certificates#Certificates for").." "..name)
  else
    form:Add("subtitle",tr("openvpn_client_type#Certificates for").." "..name)
  end
	form:Add("uci_file", "openvpn."..name..".ca", uci.get("openvpn",name,"ca"),tr("openvpn_var_ca#CA File"),"")
	form:Add("uci_file", "openvpn."..name..".cert", uci.get("openvpn",name,"cert"),tr("openvpn_var_cert#Client Certificate File"),"")
	form:Add("uci_file", "openvpn."..name..".key", uci.get("openvpn",name,"key"),tr("openvpn_var_key#Client Key File"),"")
	form:Add("uci_file", "openvpn."..name..".dh", uci.get("openvpn",name,"dh"),tr("openvpn_var_dh#Diffie Hellman (dh1024.pem)"),"")
  form:Add_help(tr("openvpn_var_SSLTLSparms#SSL/TLS parms (cert/key files)"),tr("openvpn_help_SSLTLSparms#See the server config file for more description.  It's best to use a separate .crt/.key file pair for each client.  A single ca file can be used for all clients."))
	form:Add("uci_file", "openvpn."..name..".tls_auth", uci.get("openvpn",name,"tls_auth"),tr("openvpn_var_tls_auth#TLS auth File"),"")
  form:Add_help(tr("openvpn_ns_cert_type#NS Cert type"),tr("openvpn_help_ns_cert_type#If a tls_auth key is used on the server then every client must also have the key."))
	forms = {}
	forms[#forms+1] = form
	return forms
end

--[[
function form_cert(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_certificates#Certificates").." - "..name)
  else
    form:Add("subtitle",tr("openvpn_certificates#Certificates").." - "..name)
  end
	
	if uci.get("openvpn."..name.."ca") then
		form:Add("show_text", "openvpn."..name..".ca", uci.get("openvpn", name,"ca"),tr("openvpn_var_srv_ca#CA File"),"","width:100%")
	else
		form:Add("file", "openvpn."..name..".ca", uci.get("openvpn",name,"ca"),tr("openvpn_var_srv_ca#CA File"),"","width:100%")
	end
	if uci.get("openvpn."..name..".cert") then
		form:Add("show_text", "openvpn."..name.."cert", uci.get("openvpn",name,"cert"), tr("openvpn_var_srv_ca#CA File"),"string","width:100%")
	else
		form:Add("file", "openvpn."..name.."cert", uci.get("openvpn",name,"cert"), tr("openvpn_var_srv_ca#CA File"),"string","width:100%")
	end
	form:Add("text", "openvpn."..name..".ca", uci.get("openvpn",name,"ca"),tr("openvpn_var_srv_ca#CA File"),"string","width:100%")
	form:Add("text", "openvpn."..name..".cert", uci.get("openvpn",name,"cert"),tr("openvpn_var_srv_cert#Server Certificate File"),"string","width:100%")
	form:Add("text", "openvpn."..name..".key", uci.get("openvpn",name,"key"),tr("openvpn_var_srv_key#Server Key File"),"string","width:100%")
  form:Add_help(tr("openvpn_var_SSLTLSparms#SSL/TLS parms (cert/key files)"),tr("openvpn_help_SSLTLSparms#SSL/TLS root certificate (ca), certificate (cert), and private key (key).  Each client and the server must have their own cert and key file.  The server and all clients will use the same ca file.<br>See the <strong>easy-rsa</strong> directory for a series of scripts for generating RSA certificates and private keys.  Remember to use a unique Common Name for the server and each of the client certificates.<br>Any X509 key management system can be used. OpenVPN can also use a PKCS #12 formatted key file (see <strong>pkcs12</strong> directive in man page)."))

	form:Add("text", "openvpn."..name..".dh", uci.get("openvpn",name,"dh"),tr("openvpn_var_dh#Diffie hellman parameters"),"string","width:100%")
  form:Add_help(tr("openvpn_var_dh#Diffie hellman parameters"),tr("openvpn_help_dh#Generate your own with:<br>openssl dhparam -out dh1024.pem 1024 <br>Substitute 2048 for 1024 if you are using 2048 bit keys."))
	return form
end
]]

function form_custom_cert(form,name)
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openvpn_certificates#Upload Certificates to").." /etc/openvpn/"..name)
  else
    form:Add("subtitle",tr("openvpn_certificates#Upload Certificates for").." /etc/openvpn/"..name)
  end
	form:Add("file", "uploadfile", "",tr("openvpn_var_custom_certupload#Upload Certificate"),"","width:100%")
--	form:Add("text", "openvpn."..name..".path", uci.check_set("openvpn",name,"path","/etc/openvpn/"..name),tr("openvpn_var_custom_dest#Destination Path"),"string","width:100%")
	local dir = util.fileList("/etc/openvpn/"..name)
	form:Add("list", "dir_list:openvpn", dir, tr("Certificates"),"","","",true)
	forms[#forms+1] = form
	return forms
end
