require("lua-xwrt.addon.string")
require("lua-xwrt.addon.uci")
require("lua-xwrt.html.form")
require("lua-xwrt.xwrt.translator")
require("lua-xwrt.addon.io")


--io.stderr = io.stdout
function upload(dir,file)
print(dir,file)
local algo = util.file_load("/etc/openssl/"..dir.."/"..file)
print([[Pragma: public
Expires: 0
Cache-Control: must-revalidate, post-check=0, pre-check=0
Cache-Control: private,false
Content-Description: File Transfer
Content-Type: application/octet-stream
Content-Disposition: attachment; filename="]]..file..[["
Content-Length: ]]..string.len(algo)..[[

]])
	print(algo)
	os.exit(0)
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
local __ENV = __ENV
local __FORM = __FORM
local unpack = unpack
local page = page
local upload = upload

module("lua-xwrt.openssl")
local tconf = {}
local newMenu = htmlhmenuClass.new("submenu")

function form_progress(cmdstr,title,subtitle,msg,donemsg)
	local title = title
	local subtitle = subtitle
	local msg = msg
	local donemsg = donemsg
	local cmd
	title = title or tr("process_longTimeTitle#Process Page")
	cmd = cmdstr or tr("process_longTimeCmd#Missing command")
	subtitle = subtitle or tr("process_lognTimeSubTitle#Runnig :").." "..cmd
	msg = msg or tr("process_longTimeMsg#This proccess will take a long time, please wait until appears the button to continue")
	donemsg = donemsg or tr("process_longTimeDoneMsg#Thanks for your patience<br>Process Done.")
	if cmdstr == nil then
		subtitle = tr("process_longTimeNothing#Nothing to do")
	end
--	__MENU.selected = string.gsub(__ENV.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	page.title = title
	page.action_apply = ""
	page.action_review = ""
	page.action_clear = ""
	local form = formClass.new(subtitle,true)
	for k,v in pairs(__FORM) do
		form:Add("hidden",k,v)
	end
	print(page:start())
	form:print()
--[[
print(uci.get("certificate","newcert","countryName"))
print(uci.get("certificate","newcert","stateName"))
print(uci.get("certificate","newcert","localityName"))
print(uci.get("certificate","newcert","organizationName"))
print(uci.get("certificate","newcert","organizationUnitName"))
print(uci.get("certificate","newcert","emailAddress"))
print(uci.get("certificate","newcert","commonName"))
]]
	print([[<center><div id="ad" style="width:300px;"><img id="running" src="/images/loading.gif" /><br><br>]])
	print(msg)
	print("</div></center>")
	if cmdstr ~= nil then
		local hcmd = io.popen(cmd)
		hcmd:close()
	end
	print([[<script>
	document.getElementById('ad').innerHTML="]]..donemsg..[[";
</script>]])
	print(page:the_end())
end


function form_info(file)
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	page.title = tr("cert_infoTitle#Certificate Information")
	page.action_apply = ""
	page.action_review = ""
	page.action_clear = ""
	local form = formClass.new(tr("cert_infoSubtitle#Info about").." "..file,true)
	for k,v in pairs(__FORM) do
		form:Add("hidden",k,v)
	end
	print(page:start())
	form:print()
	local hcmd = io.popen("openssl x509 -noout -text -in "..file)
	print("<pre>")
	for line in hcmd:lines() do
		print(line)
	end
	print("</pre>")
	hcmd:close()
	print(page:the_end())
end

for u, v in pairs(__FORM) do
--	print(u,v,"<br>")
	local proc, uci_var, uci_cmd, idx, uci_val = unpack(string.split(u,":"))
--	print(proc, uci_var, uci_cmd, idx, uci_val,"<br>")
	if uci_var ~= nil then
		if proc == "entity_list" then
			if uci_cmd == "del" then
				uci.delete("openssl",uci_val)
				uci.save(uci_var)
				uci.commit(uci_var)
				os.execute("rm -R /etc/openssl/"..uci_val)
			end
		end
		if proc == "certCA" then
			if uci_cmd == "new" then
				uci.check_set("certificate","newcert","entity")
				uci.set("certificate","newcert","countryName", uci.check_set("openssl",uci_var,"poly_match_countryName_default","US"))
				uci.set("certificate","newcert","stateName", uci.check_set("openssl",uci_var,"poly_match_stateOrProvinceName_default",""))
				uci.set("certificate","newcert","localityName", uci.check_set("openssl",uci_var,"poly_match_localityName_default",""))
				uci.set("certificate","newcert","organizationName", uci.check_set("openssl",uci_var,"poly_match_organizationName_default",""))
				uci.set("certificate","newcert","organizationUnitName", uci.check_set("openssl",uci_var,"poly_match_organizationUnitName_default",""))
				uci.set("certificate","newcert","emailAddress", uci.check_set("openssl",uci_var,"poly_match_emailAddress_default",""))
				local system = uci.get_type("system","system")
				uci.set("certificate","newcert","commonName", uci.check_set("openssl",uci_var,"poly_match_commonName_default",system[1].hostname))
				uci.save("certificate")
				uci.save("openssl")
				uci.commit("openssl")
				__FORM[u] = nil
				form_progress("/etc/openssl/ca-build "..uci_var,"Creating CA","Certificatin Authority")
				uci.delete("certificate","newcert")
				uci.save("certificate")
				uci.commit("certificate")
				os.exit(0)
			elseif uci_cmd == "del" then
				os.execute("rm -r /etc/openssl/"..idx)
				os.execute("mkdir -p /etc/openssl/"..idx.."/topsecret")
				os.execute("mkdir -p /etc/openssl/"..idx.."/private")
				os.execute("mkdir -p /etc/openssl/"..idx.."/newcerts")
				os.execute("mkdir -p /etc/openssl/"..idx.."/csr")
				os.execute("mkdir -p /etc/openssl/"..idx.."/crl")
				os.execute("mkdir -p /etc/openssl/"..idx.."/certs")
				os.execute("touch /etc/openssl/"..idx.."/index.txt")
				os.execute("echo '01' >> /etc/openssl/"..idx.."/serial")
				os.execute("echo '01' >> /etc/openssl/"..idx.."/crlnumber")
			elseif uci_cmd == "download" then
				upload(idx,uci_val)
			end
		end
		if proc == "cert" then
			if uci_cmd == "new" then
				uci.check_set("certificate","newcert","entity")
				uci.set("certificate","newcert","countryName", __FORM["certificate.newcert.countryName"])
				uci.set("certificate","newcert","stateName",__FORM["certificate.newcert.stateName"])
				uci.set("certificate","newcert","localityName",__FORM["certificate.newcert.localityName"])
				uci.set("certificate","newcert","organizationName",__FORM["certificate.newcert.organizationName"])
				uci.set("certificate","newcert","organizationUnitName",__FORM["certificate.newcert.organizationUnitName"])
				uci.set("certificate","newcert","emailAddress",__FORM["certificate.newcert.emailAddress"])
				uci.set("certificate","newcert","commonName",__FORM["certificate.newcert.commonName"])
				uci.save("certificate")
				__FORM[u] = nil
				form_progress("/etc/openssl/cert-build "..uci_var.." "..__FORM["certificate.newcert.commonName"],"Creating Certificate",__FORM["certificate.newcert.commonName"])
				uci.delete("certificate","newcert")
				uci.save("certificate")
				uci.commit("certificate")
				os.exit(0)
			elseif uci_cmd == "del" then
				os.execute("rm /etc/openssl/"..uci_var.."/certs/"..uci_val..".crt")
				os.execute("rm /etc/openssl/"..uci_var.."/crs/"..uci_val..".csr")
				os.execute("rm /etc/openssl/"..uci_var.."/private/"..uci_val..".key")
			elseif uci_cmd == "revoke" then
				os.execute("/etc/openssl/cert-revoke "..uci_var.." "..uci_val)
			elseif uci_cmd == "info" then
				__FORM[u] = nil
				form_info("/etc/openssl/"..uci_var)
				os.exit(0)
			elseif uci_cmd == "download" then
				upload(uci_var.."/"..idx,uci_val)
			end
		end
		if proc == "srv_cert" then
			if uci_cmd == "new" then
				if idx == "dh.pem" then
					__FORM[u] = nil
					form_progress("openssl dhparam -out /etc/openssl/"..uci_var.."/dh.pem "..uci.get("openssl",uci_var,"CA_default_bits"),"Creating Key","Diffie Hellman")
					uci.set("openvpn",uci_var,"dh","/etc/openssl/"..uci_var.."/dh.pem")
					uci.save("openvpn")
					uci.commit("openvpn")
					os.exit(0)
				end
			elseif uci_cmd == "del" then
				if idx == "dh.pem" then
					os.execute("rm /etc/openssl/"..uci_var.."/dh.pem")
					uci.delete("openvpn",uci_var,"dh","dh.pem")
					uci.save("openvpn")
					uci.commit("openvpn")
				end
			end
		end
	end
end

function new_entity(name)
	uci.check_set("openssl",name,"entity")
	uci.check_set("openssl",name,'CA_default_crldays','30')
	uci.check_set("openssl",name,'CA_default_md', 'sha1')
	uci.check_set("openssl",name,'CA_default_bits', '1024')
	uci.check_set("openssl",name,'CA_default_days', '365')

	uci.check_set("openssl",name,'poly_match_countryName', 'match')
	uci.check_set("openssl",name,'poly_match_stateOrProvinceName', 'optional')
	uci.check_set("openssl",name,'poly_match_localityName', 'optional')

	uci.check_set("openssl",name,'poly_match_organizationName', 'optional')
	uci.check_set("openssl",name,'poly_match_organizationUnitName', 'optional')

	uci.check_set("openssl",name,'poly_match_emailAddress', 'optional')
	uci.check_set("openssl",name,'poly_match_commonName', 'supplied')
	local system = uci.get_type("system","system")
	uci.set("openssl",name,'poly_match_emailAddress_default', '')
	uci.set("openssl",name,'poly_match_commonName_default', system[1].hostname)
	uci.save("openssl")
	uci.commit("openssl")
	os.execute("mkdir -p /etc/openssl/"..name.."/topsecret")
	os.execute("mkdir -p /etc/openssl/"..name.."/private")
	os.execute("mkdir -p /etc/openssl/"..name.."/newcerts")
	os.execute("mkdir -p /etc/openssl/"..name.."/csr")
	os.execute("mkdir -p /etc/openssl/"..name.."/crl")
	os.execute("mkdir -p /etc/openssl/"..name.."/certs")
	os.execute("touch /etc/openssl/"..name.."/index.txt")
	os.execute("echo '01' > /etc/openssl/"..name.."/serial")
	os.execute("echo '01' > /etc/openssl/"..name.."/crlnumber")
	__FORM.name = name
	__FORM.option = "entity"
	for k, v in pairs(__FORM) do
		if string.unescape(v) == "New Entity" then
			__FORM[k] = name
		end
	end
end

if __FORM.new_entity and __FORM.new_entity ~= "" then
	local name = __FORM.new_entity	
	new_entity(name)
end

function sslTxtInfo(file, param)
	local hfile = io.popen("openssl x509 -noout -in "..file.." "..param)
	return hfile:read("*a")
end

function file_list(path)
	local hfl = io.popen("ls "..path)
	local t = {}
	for ls in hfl:lines() do
		t[#t+1]=ls
	end
	return t
end

function getNames(common)
	local common = common
	local datos = {}
	for reg in string.gmatch(common,"[^/]+") do
		local _, _, key, val = string.find(reg,"(.+)=(.+)")
		if key then
			datos[key] = val
		end
	end
	return datos
end
	
function sslTableInfo(ffile, param, t, certDB)
	local crtTable = t or {}
	local certDB = certDB or {}
	local _, _, dir, file, ext = string.find(ffile,"(.+)/(%w+)%.(%w+)")
	
	if crtTable[file] == nil then
		crtTable[file] = {}
	end
--print(dir, file, ext, certDB[file],"<br>")	
	if certDB[file] then
		for k, v in pairs(certDB[file]) do
			crtTable[file][k] = v
		end
	end

	local hdate = io.popen("openssl x509 -noout -in "..ffile.." "..param)
	for d in hdate:lines() do
		local pos = string.find(d,"=")
		local key = string.sub(d,1,pos-1)
		local val = string.sub(d,pos+1)
		if key then
			if key == "issuer" or key == "subject" then
				crtTable[file][key] = getNames(val)
			else
				crtTable[file][key] = val
			end
		end
	end
	return crtTable
end

function readSslDb(name)
	local t = {}
	local data = util.file_load("/etc/openssl/"..name.."/index.txt")
	for line in string.gmatch(data,"[^\n]+") do
		local _, _, status, f1, f2, idx, unknow, common = string.find(line,"(%a)%s(%w+)%s(.*)%s*(%x+)%s(%w+)%s(.+)")
		if common then
			local subject = getNames(common)
			fname = subject.CN
			t[fname] = {}
			t[fname]["idx"] = idx
			t[fname]["status"] = status
			t[fname]["fecha1"] = f1
			t[fname]["fecha2"] = f2
			t[fname]["unknow"] = unknow
			t[fname]["subject"] = subject
		end
	end
	return t
end

function set_menu(t)
	local sub = "openssl.sh?option=entity&name="..t[".name"]
	local name = __FORM.name or ""
	newMenu:add(t[".name"],sub)
	if t[".name"] == __FORM.name then
		newMenu[t[".name"]] = htmlhmenuClass.new("submenu")
--		newMenu[t[".name"]]:add("Settings",sub.."&suboption=settings")
--		newMenu[t[".name"]]["Settings"] = htmlhmenuClass.new("submenu")
--		newMenu[t[".name"]]["Settings"]:add("CA default",sub.."&suboption=setting&setting=ca_def")
--		newMenu[t[".name"]]["Settings"]:add("Policy Match",sub.."&suboption=setting&setting=policy_match")
		newMenu[t[".name"]]:add("CA default",sub.."&suboption=setting&setting=ca_def")
		newMenu[t[".name"]]:add("Policy Match",sub.."&suboption=setting&setting=policy_match")
		newMenu[t[".name"]]:add("Server Certificates",sub.."&suboption=srv_cert")
--		local file = 
		if util.file_exists("/etc/openssl/"..name.."/cacert.crt") then
--			newMenu[t[".name"]]:add("CA",sub.."&suboption=ca")
			newMenu[t[".name"]]:add("Certificates",sub.."&suboption=certificates")
		end
	end
end

function init()
	newMenu:add(tr("openssl_menu_wellcome#Wellcom"),"openssl.sh")
	newMenu:add(tr("openssl_menu_new#Entities"),"openssl.sh?option=new")
	uci.foreach("openssl","entity", set_menu)
	if __MENU[__FORM.cat].len > 1 then 
		__MENU[__FORM.cat]["OpenSSL"] = newMenu
	else
		__MENU[__FORM.cat] = newMenu
	end
end

function form_wellcome()
	local forms = {}
	local str = ""
	form = formClass.new(tr("openssl_wellcome#Wellcome to OpenSSL Management"))
	str = str .. "This certificate management use this directory structure<br><br>"
	str = str .. [[<pre>
	(entity)/
	(entity)/certs
	(entity)/private
	(entity)/newcerts
	(entity)/csr
	(entity)/crl
	(entity)/OpenSSL.cnf
	(entity)/index.txt
	(entity)/serial.txt
	(entity)/crlnumber
		
		</pre>]]
	str = str .. "Under directory /etc/openssl"
	form:Add("text_line","linea1",str)
	form:Add_help(tr("openssl_welcome#Where"),tr([[openssl_help_welcome#<ul style="list-style-type:disc;font-size:70%;">
	<li><strong>certs:</strong> certificates directory container</li>
	<li><strong>csr:</strong> </li>
	<li><strong>crl:</strong> </li>
	<li><strong>index.txt:</strong> </li>
	<li><strong>newcrts:</strong> </li>
	<li><strong>private:</strong> </li>
	<li><strong>serial.txt:</strong> </li>
	<li><strong>crlnumber:</strong> </li>
	</ul>
	]]))
	forms[#forms+1] = form
	return forms
end

function joinDirUci(tdir,tuci)
	local tn = {}
	local t = {}
	if tuci then
	for _, v in ipairs(tuci) do
		tn[v[".name"]] = {}
		tn[v[".name"]]["uci"] = true
	end
	end
	if tdir then
	for _, v in ipairs(tdir) do
		if tn[v] == nil then tn[v] = {} end
		tn[v]["file"] = true
	end
	end

	for k,v in util.pairsByKeys(tn) do
		t[#t+1] = k
	end
	return t
end

function form_new(form)
	local forms = {}
	local str = ""
	form = formClass.new(tr("openssl_wellcome#New Entity"))
	form:Add("text","new_entity","",tr("openssl_entity_name#New Entity Name"))
	local dir = joinDirUci(util.dirList("/etc/openssl"),uci.get_type("openssl","entity"))
	form:Add("list", "entity_list:openssl", dir, tr("Entities List"),"","","",true)
	forms[#forms+1] = form
	return forms
end

function form_entity(form,name)
	if __FORM.suboption == "ca" then
		return form_entity_ca(form,name)
	elseif __FORM.suboption == "srv_cert" then
		return form_serverCert(form,name)
	elseif __FORM.suboption == "certificates" then
		return form_entity_certificates(form,name)
	else
		return form_entity_settings(form,name)
	end
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

function form_serverCert(form,name)
  if form == nil then
    form = formClass.new(tr("openvpn_server_type#Server Certificates").." - /etc/openssl/"..name,true)
  else
    form:Add("subtitle",tr("openvpn_server_type#Server Certificates").." - /etc/openssl/"..name)
  end
--[[
	if io.exists("/etc/openssl/"..name.."/cacert.crt") then
		if (uci.get("openvpn",name,"ca") == nil) 
		and uci.get("openvpn",name,"mode") then
			uci.set("openvpn",name,"ca","/etc/openssl/"..name.."/cacert.crt")
			uci.save("openvpn")
		end
	else
		if uci.get("openvpn",name,"ca") then
			uci.delete("openvpn",name,"ca")
			uci.save("openvpn")
		end
	end
]]
	local strh = ""
	strh = strh .. "<table width='100%' style='font-size:80%;'>\n"
	strh = strh .. "\t<tr style='background: #c8c8c8;'>"
	strh = strh ..[[<th style="width:95px;">]].."Certificate"..[[</th>]]
	strh = strh ..[[<th>]].."File"..[[</th>]]
	strh = strh ..[[<th colspan="4" style="text-align: center;" >]].."Action"..[[</th>]]
	strh = strh .. "\t</tr>\n"

	local str = strh
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."CA CRT"..[[</th>]]
	local srvfile = check_srvFile(name,"/cacert.crt")
	str = str ..[[<td>]]..srvfile..[[</td>]]
	if srvfile == "" then
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:]]..name..[[:new:cacert.crt" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	else
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[/cacert.crt:info" value="Info" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:cacert.crt]]..[[:download:]]..name..[[:cacert.crt" value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:ca:del:]]..name..[[:cacert.crt" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	end
	str = str ..[[</tr>]].."\n"
	str = str ..[[</table>]]
--	
--	str = str .. strh
--	str = str .. "\t<tr >"
--	str = str ..[[<td>]].."Server CRT"..[[</th>]]
--	srvfile = check_srvFile(name,"/certs/server.crt")
--	if srvfile == "" then
--		uci.delete("openvpn",name,"cert")
--	else
--		if uci.get("openvpn",name,"mode") then
--			uci.check_set("openvpn",name,"cert",srvfile)
--		end
--	end
--	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:certs:server" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[/certs/server.crt:info" value="Info" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[:revoke:server:server" value="Revoke" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str .."\t\t"..[[<td style="width:80px" rowspan="2">]]..[[<input type="submit" name="cert:]]..name..[[:del:server:server" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str ..[[</tr>]].."\n"
--	str = str .. "\t<tr >"
--	str = str ..[[<td>]].."Server KEY"..[[</th>]]
--	srvfile = check_srvFile(name,"/private/server.key")
--	if srvfile == "" then
--		uci.delete("openvpn",name,"key")
--	else
--		if uci.get("openvpn",name,"mode") then
--			uci.check_set("openvpn",name,"key",srvfile)
--		end
--	end
--	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str ..[[<td>]].."server.key"..[[</td>]]
--	str = str ..[[</tr>]].."\n"
--	str = str .. "\t<tr >"
--	str = str ..[[</table>]]
	
	str = str .. strh
--	str = str .. "\t<tr >"
--	str = str ..[[<td>]].."TLS Auth"..[[</th>]]
--	srvfile = check_srvFile(name,"/ta.key")
--	if srvfile == "" then
--		uci.delete("openvpn",name,"tls_auth")
--	else
--		if uci.get("openvpn",name,"mode") then
--			uci.check_set("openvpn",name,"tls_auth",srvfile)
--		end
--	end
--	str = str ..[[<td>]]..srvfile..[[</td>]]
--	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:]].."ta.key"..[[" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:ta.key]]..[[:download:]]..name..[[:ta.key"  value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:del:]].."ta.key"..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--	str = str ..[[</tr>]].."\n"
	str = str .. "\t<tr >"
	str = str ..[[<td>]].."Diffie Hellman"..[[</th>]]
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
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:new:]].."dh.pem"..[[" value="Create" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="certCA:dh.pem]]..[[:download:]]..name..[[:dh.pem" value="Download" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="srv_cert:]]..name..[[:del:]].."dh.pem"..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
	str = str ..[[</tr>]].."\n"
	str = str ..[[</table>]]
	form:Add("text_line","line1",str)
	forms = {}
	forms[#forms+1] = form
	return forms
end


function form_entity_ca(form,name)
	local form = form
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openssl_entity_ca#Certificate Authority"),true)
  else
    form:Add("subtitle",tr("openssl_entity_ca#Certificate Authority"))
  end
	local file = "/etc/openssl/"..name.."/cacert.crt"
	if util.file_exists(file) then
		form:Add("text_area","ca_cert",sslTxtInfo(file,"-text"),"CA Info","","width:100%;height:250px;")
		local str = "<table width='100%'>\n"
		str = str .. "<tr>\n"
		str = str .. "<td>If you Remove CA Certificate, will be remove all certificates signed by this CA<td>\n"
----------------------------------------
		str = str .. "<td><input type='submit' name='certCA:ca:del:"..name..":cacert.crt' value='Remove it' /></td>\n"
		str = str .. "<td><input type='submit' name='certCA:ca:download:"..name..":cacert.crt' value='Download It' /></td>\n"
-----------------------------------------
		str = str .. "</tr>"
		str = str .. "</table>"
		form:Add("show_text","text1",str,"","")
	else
		createCA(form,name)
	end
	forms[#forms+1] = form
	return forms
end

function createCA(form,name)
	form:Add("submit","certCA:"..name..":new","Create CA",tr("openssl_var_newCA#Create New CA"),"", "width:147px;")
end

function htmlTableCerts(crtsList,name)
	local str = ""
	str = str .. "<table width='100%' style='font-size:80%;'>\n"
	str = str .. "\t<tr style='background: #c8c8c8;'>"
	str = str ..[[<th style="width:10px">]].."S"..[[</th>]]
	str = str ..[[<th style="width:25px">]].."Idx"..[[</th>]]
	str = str ..[[<th>]].."Name"..[[</th>]]
	str = str ..[[<th style="width:190px">]].."Not Before"..[[</th>]]
	str = str ..[[<th style="width:190px">]].."Not After"..[[</th>]]
	str = str ..[[<th colspan="3" style="width:75px">]].."Download"..[[</th>]]
	str = str ..[[<th colspan="3" style="text-align: center;" >]].."Action"..[[</th>]]
	str = str ..[[</tr>]].."\n"
	for f, data in util.pairsByKeys(crtsList) do
		str = str .. "\t<tr>\n"
		str = str .."\t\t"..[[<td>]]..(data.status or "&nbsp;")..[[</td>]].."\n"
		if data.idx then
			str = str .."\t\t"..[[<td align="right">]]..(data.idx or "&nbsp;")..[[</td>]].."\n"
		else
			str = str .."\t\t"..[[<td align="right">&nbsp;</td>]].."\n"
		end
		str = str .."\t\t"..[[<td>]]..f..[[</td>]].."\n"
		if data["notBefore"] then
			str = str ..[[<td>]]..data["notBefore"]..[[</td>]]
			str = str ..[[<td>]]..data["notAfter"]..[[</td>]]
		else
			str = str ..[[<td align="right">&nbsp;</td>]]
			str = str ..[[<td align="right">&nbsp;</td>]]
		end

		str = str .."\t\t"..[[<td style="width:25px;">]]..[[<input type="submit" name="cert:]]..name..[[:download:certs:]]..f..[[.crt" value="crt" style="font-size:80%;width:25px;"/> </td>]].."\n"
		str = str .."\t\t"..[[<td style="width:25px;">]]..[[<input type="submit" name="cert:]]..name..[[:download:private:]]..f..[[.key" value="key" style="font-size:80%;width:25px;"/> </td>]].."\n"
		str = str .."\t\t"..[[<td style="width:25px;">]]..[[<input type="submit" name="cert:]]..name..[[:download:csr:]]..f..[[.csr" value="csr" style="font-size:80%;width:25px;"/> </td>]].."\n"

		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[/certs/]]..f..[[.crt:info" value="Info" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
--		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[:renew:]]..f..[[" value="Renew" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[:revoke:all:]]..f..[[" value="Revoke" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t\t"..[[<td style="width:80px">]]..[[<input type="submit" name="cert:]]..name..[[:del:all:]]..f..[[" value="Remove" style="font-size:80%;width:80px;"/>]]..[[</td>]].."\n"
		str = str .."\t"..[[</tr>]].."\n"
	end
	str = str .."</table>\n"
	return str
end

function form_entity_certificates(form,name)
	local name = name
	local form = form
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openssl_entity_ca#Certificates of").." "..name,true)
  else
    form:Add("subtitle",tr("openssl_entity_ca#Certificates of").." "..name)
  end
	local crtsList = readSslDb(name)
	local filetb = file_list("/etc/openssl/"..name.."/certs/*")
	local t = {}
	local str = ""
--	os.execute("touch /etc/config/certificate")
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

	if #filetb > 0 then
		for k, file in ipairs(filetb) do
			sslTableInfo(file,"-dates", t, crtsList)
		end
		str = str .. htmlTableCerts(t,name)
	end
	form:Add("text_line","linea1",str)
--	local str1 = util.table2string(t,"<br>")
--	form:Add("text_line","linea2",str1)

	forms[#forms+1] = form
	return forms
end
	
function form_entity_settings(form,name)
	forms = {}
	if __FORM.setting == "policy_match" then
		forms[#forms+1] = form_ent_set_policy_match(form,name)
		forms[#forms+1] = form_ent_set_policy_match_values(form,name)
	else
		forms[#forms+1] = form_ent_set_ca_def(form,name)
	end
	uci.save("openssl")
	uci.commit("openssl")
	return forms
end

function form_ent_set_policy_match(form,name)
	local form = form
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openssl_set_poli_matc#Policy Match Settings"))
  else
    form:Add("subtitle",tr("openssl_set_poli_matc#Policy Match Settings"))
  end
	form:Add("select","openssl."..name..".poly_match_countryName",uci.check_set("openssl",name,"poly_match_countryName","match"),tr("openssl_var_countryName#Country Name"),"string")
	form["openssl."..name..".poly_match_countryName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_countryName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_countryName"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_stateOrProvinceName",uci.check_set("openssl",name,"poly_match_stateOrProvinceName","optional"),tr("openssl_var_stateOrProvinceName#Province or State"),"string")
	form["openssl."..name..".poly_match_stateOrProvinceName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_stateOrProvinceName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_stateOrProvinceName"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_localityName",uci.check_set("openssl",name,"poly_match_localityName","optional"),tr("openssl_var_localityName#Locality Name"),"string")
	form["openssl."..name..".poly_match_localityName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_localityName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_localityName"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_organizationName",uci.check_set("openssl",name,"poly_match_organizationName","optional"),tr("openssl_var_organizationName#Organization Name"),"string")
	form["openssl."..name..".poly_match_organizationName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_organizationName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_organizationName"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_organizationUnitName",uci.check_set("openssl",name,"poly_match_organizationUnitName","optional"),tr("openssl_var_organizationUnitName#Organization Unit"),"string")
	form["openssl."..name..".poly_match_organizationUnitName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_organizationUnitName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_organizationUnitName"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_emailAddress",uci.check_set("openssl",name,"poly_match_emailAddress","optional"),tr("openssl_var_emailAddress#e-Mail Address"),"string")
	form["openssl."..name..".poly_match_emailAddress"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_emailAddress"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_emailAddress"].options:Add("optional",tr("optional"))

	form:Add("select","openssl."..name..".poly_match_commonName",uci.check_set("openssl",name,"poly_match_commonName","supplied"),tr("openssl_var_commonName#Common Name"),"string")
	form["openssl."..name..".poly_match_commonName"].options:Add("match",tr("match"))
	form["openssl."..name..".poly_match_commonName"].options:Add("supplied",tr("supplied"))
	form["openssl."..name..".poly_match_commonName"].options:Add("optional",tr("optional"))

	return form
end

function form_ent_set_policy_match_values(form,name)
	local form = form
  if form == nil then
    form = formClass.new(tr("openssl_set_poli_matc#Policy Default Values"))
  else
    form:Add("subtitle",tr("openssl_set_poli_matc#Policy Default Values"))
  end
--	local sType = uci.get("openssl",name,poly_match_countryName)
--	if sType == "match" or sType == "supplied" then
		form:Add("text","openssl."..name..".poly_match_countryName_default",uci.check_set("openssl",name,"poly_match_countryName_default","US"),tr("openssl_var_countryName#Country Name"),"string")
--	end
--	sType = uci.get("openssl",name,poly_match_stateOrProvinceName)
	form:Add("text","openssl."..name..".poly_match_stateOrProvinceName_default",uci.check_set("openssl",name,"poly_match_stateOrProvinceName_default","Province Or State"),tr("openssl_var_stateOrProvinceName#Province or State"),"string")
	form:Add("text","openssl."..name..".poly_match_localityName_default",uci.check_set("openssl",name,"poly_match_localityName_default","City or some Location "),tr("openssl_var_localityName#Locality Name"),"string")
	form:Add("text","openssl."..name..".poly_match_organizationName_default",uci.check_set("openssl",name,"poly_match_organizationName_default","Company Name"),tr("openssl_var_organizationName#Organization Name"),"string")
	form:Add("text","openssl."..name..".poly_match_organizationUnitName_default",uci.check_set("openssl",name,"poly_match_organizationUnitName_default","Unit or Deparment Name"),tr("openssl_var_organizationUnitName#Organization Unit"),"string")
	form:Add("text","openssl."..name..".poly_match_emailAddress_default",uci.check_set("openssl",name,"poly_match_emailAddress_default",""),tr("openssl_var_emailAddress#e-Mail Address"),"string")
	form:Add("text","openssl."..name..".poly_match_commonName_default",uci.check_set("openssl",name,"poly_match_commonName_default","HostName or IP or your name"),tr("openssl_var_commonName#Common Name"),"string")
	local file = "/etc/openssl/"..name.."/cacert.crt"
	if not util.file_exists(file) then
		createCA(form,name)
	end
	return form
end

function form_ent_set_ca_def(form,name)
	local form = form
	local forms = {}
  if form == nil then
    form = formClass.new(tr("openssl_ca_def#CA default"))
  else
    form:Add("subtitle",tr("openssl_ca_def#CA defaul"))
  end
	form:Add("text","openssl."..name..".CA_default_days", uci.check_set("openssl",name,"CA_default_days",365), tr("openssl_var_days#Default Days"),"int")
	form:Add("text","openssl."..name..".CA_default_crldays", uci.check_set("openssl",name,"CA_default_crldays",30), tr("openssl_var_crldays#Default CRL Days"),"int")
--	form:Add("text","openssl."..name..".CA_default_bits", uci.check_set("openssl",name,"CA_default_bits",1024), tr("openssl_var_bits#Default Bits"),"int")
	form:Add("select","openssl."..name..".CA_default_bits",uci.check_set("openssl",name,"CA_default_bits","1024"),tr("openssl_var_bits#Defalt Bits"),"string")
	form["openssl."..name..".CA_default_bits"].options:Add("512","512")
	form["openssl."..name..".CA_default_bits"].options:Add("1024","1024")
	form["openssl."..name..".CA_default_bits"].options:Add("2048","2048")
	form["openssl."..name..".CA_default_bits"].options:Add("4096","4096")
	form:Add("select","openssl."..name..".CA_default_md",uci.check_set("openssl",name,"CA_default_md","sha1"),tr("openssl_var_md#Msg Digest algorithm"),"string")
	local mdlist = io.popen("openssl list-message-digest-commands -text")
	for m in mdlist:lines() do
		form["openssl."..name..".CA_default_md"].options:Add(m,m)
	end
	forms[#forms+1] = form
	return form
end
---------------------------------------------------------
