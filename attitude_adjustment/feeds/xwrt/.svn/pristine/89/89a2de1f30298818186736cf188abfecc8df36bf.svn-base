require("lua-xwrt.addon.uci")
require("lua-xwrt.html.form")
require("lua-xwrt.xwrt.page")

changes_uciClass = {} 
changes_uciClass_mt = {__index = changes_uciClass} 

function changes_uciClass.new () 
	local self = {}
	self.count = 0;
	self.toapply = {}
	setmetatable(self,changes_uciClass_mt) 
	self:Update()
	return self 
end 

function changes_uciClass:Update()
	for key, value in pairs(__FORM) do
		if string.match(key,"val_str_") then __TOCHECK[#__TOCHECK+1]=string.sub(key,9) end
		if string.match(key,"UCI_CMD_") then
			__UCI_CMD[#__UCI_CMD+1]={}
			__UCI_CMD[#__UCI_CMD]["cmd"] = string.sub(key,9)
			__UCI_CMD[#__UCI_CMD].cmd = string.sub(__UCI_CMD[#__UCI_CMD].cmd,1,3)
			if __UCI_CMD[#__UCI_CMD].cmd == nil or __UCI_CMD[#__UCI_CMD].cmd == "" then
				__UCI_CMD[#__UCI_CMD].cmd = "show"
			end
			__UCI_CMD[#__UCI_CMD].varname = string.sub(key,12)
			__UCI_CMD[#__UCI_CMD].value = value 
		end
		if string.match(key, "UCI_MSG_") then
			__UCI_MSG[#__UCI_MSG+1] = {}
			__UCI_MSG[#__UCI_MSG]["cmd"] = string.sub(key,9,11)
--			__UCI_MSG[#__UCI_MSG]["cmd"] = string.sub(__UCI_MSG[#__UCI_MSG]["cmd"],9,11)
			__UCI_MSG[#__UCI_MSG]["var"] = string.sub(key,12)
			__UCI_MSG[#__UCI_MSG]["val"] = value
		end
	end
	for i=1, #__UCI_MSG do
		if __UCI_MSG[i]["cmd"] == "set" then
			uci.set(__UCI_MSG[i]["var"].."="..__UCI_MSG[i]["val"])
		elseif __UCI_MSG[i]["cmd"] == "add" then
			uci.add(__UCI_MSG[i]["var"].."="..__UCI_MSG[i]["val"])
		elseif __UCI_MSG[i]["cmd"] == "del" then
			uci.delete(__UCI_MSG[i]["var"])
		else
			print("Error :"..__UCI_MSG[i]["cmd"],__UCI_MSG[i]["var"],__UCI_MSG[i]["val"])
		end
	end
	for i=1, #__UCI_CMD do
		if __UCI_CMD[i].cmd == "snw" and __FORM.UCI_SET_VALUE ~= "" then
			__UCI_CMD[i].cmd = "set"
			__UCI_CMD[i].value = __UCI_CMD[i].value ..":"..__FORM.UCI_SET_VALUE
		end
		if __UCI_CMD[i].cmd == "set" then
			local grp, name = unpack(string.split(__UCI_CMD[i].value,":"))
			if name == nil then name = "" end
			if __UCI_VERSION == nil then	
				assert(os.execute("mkdir /tmp/.uci > /dev/null 2>&1"))
				os.execute("echo \"config '"..grp.."' '"..name.."'\" >>/tmp/.uci/"..__UCI_CMD[i].varname)
			else
				if name == "" then
					uci.add(__UCI_CMD[i].varname,grp)
				else
					uci.set(__UCI_CMD[i].varname,name,grp)
				end
			end
		elseif __UCI_CMD[i].cmd == "del" then
			if __UCI_VERSION == nil then 
				os.execute("uci "..__UCI_CMD[i].cmd.." "..__UCI_CMD[i].varname)
			else
				uci.delete(__UCI_CMD[i].varname)
			end
		end
	end

	local saveList = {}
	for i, v in ipairs(__TOCHECK) do
		local uciparam = {}
		for value in string.gmatch(v,"[^.]+") do
			uciparam[#uciparam+1] = value
		end
		local uci_value = uci.get(v)
		if uci_value == nil then uci_value = "" end
		if __FORM[v] == nil then __FORM[v] = "" end
		local error = validate(__FORM["val_lbl_"..v],__FORM[v],__FORM["val_str_"..v],v)
		if error ~=nil then __ERROR[#__ERROR+1] = error end
		if __FORM[v] ~= uci_value and error==nil then
			if __FORM[v] == "" then 
				uci.delete(v)
			else
				local okset = uci.set(uciparam[1],uciparam[2],uciparam[3],__FORM[v])
			end
			saveList[uciparam[1]] = "to_save"
		end
	end
	for s, v in pairs(saveList) do
		uci.save(s)
	end
	self:count_changes()
end

function changes_uciClass:clear()
	page.title = tr("Clear Changes").." ("..self.count..")"
	page.action_apply = ""
	page.action_review = ""
	page.action_clear = ""
	print(page:start())
	self:show_changes()
	print(page:the_end())
	os.execute("rm /tmp/.uci/*")
	os.exit()
end

function changes_uciClass:show()
	page.title = tr("Review Changes").." ("..self.count..")"
	page.action_review = ""
--	page.action_apply = ""
--	page.action_clear = ""
	print(page:start())
	self:show_changes()
	print(page:the_end())
	os.exit()
end

function changes_uciClass:apply()
	__MENU.selected = string.gsub(__ENV.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	page.title = tr("Apply Changes").." ("..self.count..")"
	page.action_apply = ""
	page.action_review = ""
	page.action_clear = ""
	print(page:start())
	dofile("/usr/lib/lua/lua-xwrt/xwrt/apply.lua")
	print(page:the_end())
	os.exit()
end

function changes_uciClass:count_changes()
	for k, t in pairs(uci.changes()) do
		if k then
			if self.toapply[k] == nil then self.toapply[k]= {} end
			local form = formClass.new(k,true)
			for i, v in pairs(t) do
				if v[".type"] then
					self.toapply[k][k.."."..i] = v[".type"]
					self.count = self.count + 1
				end
				for n, l in pairs(v) do
					if string.sub(n,1,1) ~= "." then
						self.toapply[k][k.."."..i.."."..n] = l
						self.count = self.count + 1
					end
				end
			end
		end
	end
end

function changes_uciClass:show_changes()
	__MENU.selected = string.gsub(__ENV.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	for k, t in pairs(self.toapply) do
		local form = formClass.new(k,true)
		for i, v in pairs(t) do
			form:add("text_line",i,i.."="..v)
		end
		form:print()
	end
end






--[[


function changes_uciClass:readUpdated()
	self["count"] = 0 --uci.changes()
	local BUFSIZE = 2^13     -- 8K
	assert(os.execute("mkdir /tmp/.uci > /dev/null 2>&1"))
	local filelist = assert(io.popen("ls /tmp/.uci")) 
	for filename in filelist:lines() do
		local lc = 0
		local f = io.input("/tmp/.uci/"..filename)   -- open input file
		self[filename]={}
		while true do
			local lines, rest = f:read(BUFSIZE, "*line")
			if not lines then break end
			if rest then lines = lines .. rest .. '\n' end
			for li in string.gmatch(lines,"[^\n]+") do
				self[filename][#self[filename]+1] = li
				self["count"] = self["count"] + 1
			end
		end
	end
end

function changes_uciClass:review()
  self:readUpdated()
	__MENU.selected = string.gsub(__SERVER.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.title = tr("Review Changes").." ("..self.count..")"
--	page.action_apply = ""
	page.action_review = ""
--	page.action_clear = ""
	
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	print(page:header())
	for k,t in pairsByKeys(self) do
		if type(t) == "table" then
			local form = formClass.new(k,true)
			print (form:startFullForm())
			for i, linea in pairs(t) do
				print (linea,"<br>")
			end
			print (form:endForm())
		end
	end
	print(page:footer())
	os.exit()
end

function changes_uciClass:apply()
  self:readUpdated()
  dofile("/usr/lib/lua/lua-xwrt/apply.lua")
--  require("apply.lua")
	os.exit()
end

function changes_uciClass:clear()
  self:readUpdated()
	__MENU.selected = string.gsub(__SERVER.REQUEST_URI,"(.*)_changes&(.*)","%2")
	page.title = tr("Clear Changes").." ("..self.count..")"
--	page.action_apply = ""
	page.action_review = ""
--	page.action_clear = ""
	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
	print(page:header())
	for k,t in pairsByKeys(self) do
		if type(t) == "table" then
			local form = formClass.new(k,true)
			print (form:startFullForm())
			for i, linea in pairs(t) do
				print (linea,"... deleted...<br>")
			end
			print (form:endForm())
		end
	end
	print(page:footer())
	os.execute("rm /tmp/.uci/*")
	os.exit()
end

]]