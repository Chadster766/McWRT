require("lua-xwrt.addon.string")
cgi = require("lua-xwrt.xwrt.cgi")
--[[
function get_env()
	local t = {}
  local  myenv = io.popen("env")
  for line in myenv:lines() do
		_, _, key, value = string.find(line, "([A-Z_0-9a-z]+)[%=]+(.*)")
--print(line, key, value)
		t[key] = value
	end
--print("-------------------------------------------------------")
	return t
end

function processInfo(str,data)
	local lines = string.split(str,"\r\n")
	local retKey = ""
	local retData = {}
	local count = 0
	for i=1, #lines do
		local fields = string.split(lines[i],"; ")
		_, _, content, ctype = string.find(fields[1],"(.+): (.+)")
		if string.lower(content) == "content-disposition" then
			for n=2, #fields do
				local _, _, key, value = string.find(fields[n],"(.+)=(.+)")
				if value then value = string.gsub(value,'"',"") end
				if key == "name" then
					retKey = value
				else
					retData[key]=value
					count = count+1
				end
			end
			retData["data"]=data
			count = count+1
		end
	end
	if count == 1 then
		return retKey, data
	end
	return retKey, retData
end

function get_data(str)
	local i, e = string.find(str,"\r\n\r\n")
	local data = string.sub(str,e+1)
	local key
	key, data = processInfo(string.sub(str,1,i-1), data)
	return key, data
end

function process_post(data)
	local t = {}
	local ini, pos = string.find(data,"\r\n")
	local sepend = string.sub(data,1,ini-1)
	ini = 1
	local pos, ini = string.find(data, sepend, ini)
	ini = ini+2
	while true do
		local pos, pend = string.find(data, sepend, ini)
		if pos == nil then break end
		local str=string.sub(data,ini,pos-3)
		local key, value = get_data(str)
		t[key] = value
		ini = pend+3
	end
	return t
end

function get_post()
	local post = {}
	local char = string.char(255)
	local lowchar = string.char(0)
	local data = os.getenv("QUERY_STRING")
	local method = os.getenv("REQUEST_METHOD")
	if method == nil then return {} end
	if method == "GET" then char = "=" end
	local key, value
	if method == "POST" then
		data = io.stdin:read"*a"
		return process_post(data)
	end
	for l in string.gmatch(data,"[^&]+") do
		l = string.gsub(l,"["..char.."]%s+",char)
		local _, _, key, value = string.find(l, "(.+)%s*["..char.."]%s*(.*)")
		key = string.trim(key)
		value = string.trim(value)
		if key ~= nil then
			post[key]=value
		end
	end
	return post
end
]]
__ENV = cgi.env
__FORM = cgi.params
for k, v in pairs(__FORM) do
	local msg, service, action = unpack(string.split(k,":"))
	if msg == "service" then
		os.execute("/etc/init.d/"..service.." "..action )
--		os.execute("sleep 3")
	end
end	