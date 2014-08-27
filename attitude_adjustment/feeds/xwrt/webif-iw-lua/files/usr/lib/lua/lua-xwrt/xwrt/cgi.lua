require("lua-xwrt.addon.string")
local string = string
local io = io
local ipairs = ipairs
local print = print
local arg = arg

module("lua-xwrt.xwrt.cgi")
local function get_env()
	local t = {}
	local  myenv = io.popen("env")
	for line in myenv:lines() do
		_, _, key, value = string.find(line, "([A-Z_0-9a-z]+)[%=]+(.*)")
		t[key] = value
	end
	return t
end

local function getboundary ()
	local content_type, boundary = "", ""
	if env["CONTENT_TYPE"] then
		_,_,content_type, boundary = string.find (env["CONTENT_TYPE"], "(%S+)%s*boundary%=(.-)$")
		cotent_type = content_type or ""
		if boundary then boundary = "--"..boundary
		else boundary = "" end
	end
	return content_type, boundary
end

local function post()
	local data = io.stdin:read("*a")
	local params = {}
	for k,v in ipairs(string.split(data,boundary.."\r\n")) do
		local t = {}
		string.gsub(v,'([^%c%s:]+):%s+.+;%s*[^%s=]+="(.-)"%c*(%C*)', 
			function(a,b,c)
				a = string.trim(a)
				b = string.trim(b)
				c = string.trim(c)
				t["content"] = string.lower(a)
				t["name"] = b
				t["value"] = c
				if b then params[b] = c	end
			end)
--		print(t.content, t.name, t.value)
	end
	return params
end

params = {}
env = get_env()
content_len = env["CONTENT_LENGTH"] or env["CONTENT_LENGTH"]
content_type, boundary = getboundary()
request_method = env["REQUEST_METHOD"] or env["REQUEST_METHOD"]

if request_method == "POST" then
	params = post()
elseif request_method == "GET" then
	for line in string.gmatch(env.QUERY_STRING,"[^%&]+") do
		local _, _, key, value = string.find(line,"([^%=]+)[%=]([^%=]*)")
		params[key] = value
	end
else
--[[
	if #arg > 0 then
	for i=1, #arg do
		for line in string.gmatch(arg[i],"[^%&%s]+") do
			local _, _, key, value = string.find(line,"([^%=]+)[%=]([^%=]*)")
			params[key] = value
		end
	end
	end
]]
end
