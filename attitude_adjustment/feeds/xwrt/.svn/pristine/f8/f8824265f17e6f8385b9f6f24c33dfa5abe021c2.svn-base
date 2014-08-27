util = require("lua-xwrt.xwrt.util")
require("lua-xwrt.addon.uci")
require("lua-xwrt.addon.string")

htmlhmenuClass = {} 
htmlhmenuClass_mt = {__index = htmlhmenuClass} 

function htmlhmenuClass.new(class)
	self = {}
	auth = {}
	self.noauth = false
	self.user = __ENV.REMOTE_USER or __ENV.USER or "unknow"
	self.len = 0
	self.class = class or "mainmenu"
	self.selected =""
	self.sel_cat = "Info"
	setmetatable(self,htmlhmenuClass_mt)
	return self 
end

function htmlhmenuClass:add(name,val)
	if val == nil then val = "" end
	if self[name]==nil or name == "-" then
		self[#self+1] = {}
		self[#self]["name"] = name
		self[#self]["value"]= val
		self[name]=self[#self]
		self.len = self.len + 1
	else
		self[name].value = val
	end
end

function htmlhmenuClass:checkAuth(cat,ord)
	if self.user == "root" or usr == "admin" then return true end
	if uci.get("webif_access_control", self.user, cat.."_"..ord) == "1" then return true end
	return false
end

function htmlhmenuClass:MakeFileMenu()
	local ts = {}
	for k, f in pairs(util.fileList("/www/cgi-bin/webif")) do
		local data = util.file_load("/www/cgi-bin/webif/"..f)
		local __, __, cat, ord, sub = string.find(data, "##WEBIF:name:(.+):(.+):([A-Za-z0-9#_%s]+)\n")
		if cat and ord and sub then
			if self:checkAuth(cat,ord) then
				if ts[cat] == nil then ts[cat] = {} end
				if ts[cat][ord] == nil then ts[cat][ord] = {} end
				ts[cat][ord][sub] = "/cgi-bin/webif/"..f
			end
		end
	end
	local hcat = io.open("/www/cgi-bin/webif/.categories")
	local scat = ""
	local ssub = ""
	for cat in hcat:lines() do
		local __, __, cat = string.find(cat,"##WEBIF:category:(.+)")
		if cat == "-" then
			scat = scat.."-:-\n"
		else
			if ts[cat] then
--			scat = scat..cat
				local i = false
				for k, v in util.pairsByKeys(ts[cat]) do
					local s = next(v)
					if i == false then
						scat = scat..cat..":"..v[s].."\n"
						i = true
					end
					ssub = ssub..cat..":"..k..":"..s..":"..v[s].."\n"
				end
			end
		end
	end
	hcat:close()
	hcat = io.open("/var/.webcache/cat_"..self.user,"w")
	if hcat then 
		hcat:write(scat)
		hcat:close()
	end
	hcat = io.open("/var/.webcache/subcat_"..self.user,"w")
	if hcat then
		hcat:write(ssub)
		hcat:close()
	end
end

function htmlhmenuClass:loadXWRT()
	if self.user == nil then
		return
	end
	if not util.file_exists("/var/.webcache/cat_"..self.user) then
		self:MakeFileMenu()
	end
	local tc = util.file2table("/var/.webcache/cat_"..self.user)
	local ts = util.file2table("/var/.webcache/subcat_"..self.user)
	local tmp = {}
	self.sel_cat = __FORM.cat or "Info"
	if tc then
	for i=1, #tc do
		category, script = unpack(string.split(tc[i],":"))
		self:add(category, script)
		auth[script] = category
	end
	self[self.sel_cat] = htmlhmenuClass.new("submenu")
	for i=1, #ts do
		category, order, subcat, script = unpack(string.split(ts[i],":"))
		if self.sel_cat == category then
			if script == __ENV.SCRIPT_NAME then
				self[self.sel_cat].sel_sub = subcat
			end
			self[self.sel_cat]:add(subcat, script)
			auth[script] = subcat
		end
	end
	end
end

function htmlhmenuClass:text(tmenu, level, scgi)
	local tmenu = tmenu or self
	local submenu = nil
	local level = level or 0
	local scgi = scgi or ""
	local selected = "cat"
	local __strMenu = ""
	local cgi = ""
	if tmenu ~= self then 
		selected ="submenu"..level
	end
	__strMenu =  "<div id=\""..tmenu.class.."\">\n\t<ul>\n"
	for zz, t in ipairs(tmenu) do
		if t.name == "-" and t.value == "-" then
			__strMenu = __strMenu .."\t\t<li class=\"separator\">&nbsp;</li>\n"
		else
--			cgi = selected.."="..string.escape(t.name)
			cgi = selected.."="..t.name
			if scgi ~= "" then cgi = "&"..cgi end
			local inc = "?"
			if t.value == "" then
				local myMenu = tmenu
				local tt = t
				local idx = zz
				repeat 
					local nxtt = myMenu[tt.name][1]
					t.value = nxtt.value
--					print(zz,"name"..nxtt.name,"script"..nxtt.value,"name"..tt.name,"script"..tt.value)
					myMenu = myMenu[tt.name]
					tt = myMenu[1]
--					print(zz,"name"..nxtt.name,"script"..nxtt.value,"name"..tt.name,"script"..tt.value)
--					print(t.value,tt)
--[[
					for k, v in pairs(myMenu) do
						print(k,v)
						if type(v) == "table" then
							for n,u in pairs(v) do
								print("",n,u)
								if type(u) == "table" then
									for i,w in pairs(u) do
										print("","",i,w)
									end
								end
							end
						end
					end
]]
				until t.value ~= "" or tt == nil
--				os.exit(0)
				if t.value == "" then
					t.value = "Error to find option"
				end
			end
			if string.match(t.value,"[?]") then 
				inc = "&"
			end
--			__FORM[selected] = __FORM[selected] or string.escape(t.name)
			__FORM[selected] = __FORM[selected] or t.name
			
			if string.unescape(__FORM[selected]) == t.name then
				__strMenu = __strMenu..'\t\t<li class="selected"><a href="'..t.value..inc..scgi..cgi..'">'..tr(t.name)..'</a></li>\n'
				self.selected = string.format("%s",t.value..inc..scgi..cgi)
			else
				__strMenu = __strMenu..'\t\t<li><a href="'..t.value..inc..scgi..cgi..'">'..tr(t.name)..'</a></li>\n'
			end
		end
		if tmenu[t.name].class and string.unescape(__FORM[selected]) == t.name then
			submenu = tmenu[t.name]
			if scgi == "" then
				scgi = selected.."="..t.name
			else
				scgi = scgi.."&"..selected.."="..t.name
			end
		end
--		print(t.name, t.value, level, scgi,"<br>")
	end
	__strMenu = __strMenu.."\t</ul>\n</div><!-- "..tmenu.class.." -->\n"
	if submenu then
		__strMenu = __strMenu .. self:text(submenu, level+1, scgi)
	end
	local n = string.find(self.selected, "?")
--	print(n)
	if n then self.selected = string.sub(self.selected, n+1) end
--	print(self.selected,"<br>")
	return __strMenu
end

function htmlhmenuClass:permission()
	if page.noauth == true then return true end
	if user == "root"
	or user == "admin" then 
		return true 
	end
	if auth[__ENV.SCRIPT_NAME] ~= nil
	then return true end
	if auth[__ENV.REQUEST_URI] ~= nil
	then return true end
--[[
	for s, v in pairs(auth) do
		if string.match(s,__ENV.SCRIPT_NAME) then 
			return true 
		end
	end
]]
	page.title = tr("Permision Denied")
--	__MENU.selected = string.gsub(__ENV.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.savebutton =""--<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	page.action_review = ""
	page.action_apply = ""
	page.action_clear = ""
	print(page:start())
	print(page:the_end())
	os.exit()
end

function htmlhmenuClass:Denied()
	if noauth == true then return true end
	print("Permission Denied<br>")
--	page.title = tr("Review Changes").." ("..self.count..")"
--	__MENU.selected = string.gsub(__ENV.REQUEST_URI,"(.*)_changes&(.*)","%2")
--	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	page.action_review = ""
	page.action_apply = ""
	page.action_clear = ""
	print(page:start())
	page.content:Add("Permission Denied")
	page.content:Add(__ENV.SCRIPT_NAME)
	page.content:Add(__ENV.REQUEST_URI)
	print(page:the_end())
	os.exit()
end