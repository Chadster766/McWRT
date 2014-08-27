require("webif2.addon.string")
require("webif2.xwrt.translator")

local string = string
local io = io
local os = os
local pairs, ipairs = pairs, ipairs
local table = table

local type = type
local print = print
local tostring, tonumber = tostring, tonumber
local __TOCHECK = __TOCHECK
local __UCI_CMD = __UCI_CMD
local __ERROR = __ERROR
local __FORM = __FORM
local assert = assert
local loadstring = loadstring

local tr = tr
local page = page

module("lua-xwrt.xwrt.util")
function show_table(t,idx)
	local idx = idx or 0
	local str = string.rep("  ",idx)
	for k, v in pairs(t) do
		if type(v) == "table" then
			print(str..k)
			show_table(v,idx+1)
		else
			print(str..tostring(k).." => "..tostring(v))
		end
	end
end

function intdiv(a,b)
	a = tonumber(a) or 0
	b = tonumber(b) or 0
	if a == 0 then return 0 end
	if b == 0 then return a end
	local c = (a % b)
	if c == 0 then 
		return a/b, 0
	else 
		return ((a-c)/b), c
	end
end


-- Load File
function file_load(filename)
	local data = ""
	local error = ""
	local BUFSIZE = 2^15     -- 32K
	local f = file_exists( filename )
	if f == true then
    f = assert(io.open(filename,"r"))   -- open input file
  else f = false end

	if f then 
    while true do
		  local lines, rest = f:read(BUFSIZE, "*line")
		  if not lines then break end
		  if rest then lines = lines .. rest .. '\n' end
      data = data ..lines
    end
  else
    return "No such file or directory", 0
	end
	return data, string.len(data)
end

function file_exists( file )
    local f = io.open( file, "r" )
    if f then
        io.close( f )
        return true
    else
        return false
    end
end

function file2table(filename,clean)
  local t = {}
	local f = file_exists( filename )
	if f == false then return nil, "file do not exists" end
	f = io.open(filename,"r")
	for line in f:lines() do
		t[#t+1] = line
	end
  return t
end

function uptime()
	t = {}
	info = io.popen("uptime")
	for linea in info:lines() do
		linea = string.gsub(linea,".+ up ([0-9 a-z]) ","%1")
		local i,e = string.find(linea,", load average: ")
		t["loadavg"]=string.sub(linea,e)
		t["uptime"]=string.sub(linea,0,i-1)
	end		
	info:close()
	return t
end

function isEnabled(str)
	local ls = io.popen("ls /etc/rc.d")
	for line in ls:lines() do
		if string.match(line,str) then
			return true
		end
	end
	return false
end

function isRunning(str)
	local ls = io.popen("ps")
	for line in ls:lines() do
		if string.match(line,str) then
			return true, line
		end
	end
	return false, "Not running"
end

-- To read a tables sort by Keys
function pairsByKeys (t, f)
	local a = {}
	for n in pairs(t) do table.insert(a, n) end
		table.sort(a, f)
		local i = 0      -- iterator variable
		local iter = function ()   -- iterator function
		i = i + 1
		if a[i] == nil then return nil
		else return a[i], t[a[i]]
		end
	end
	return iter
end

function table2string(t,nl,idx)
	local idx = idx or 0
	local str = string.rep("  ",idx)
	local ret = ""
	for k, v in pairs(t) do
		if type(v) == "table" then
			ret = ret .. str .. "start_" .. k .. nl
			ret = ret .. table2string(v,nl,idx+1)
			ret = ret .. str .. "end_" .. k .. nl
		else
			ret = ret .. str..tostring(k).." => "..tostring(v)..nl
		end
	end
	return ret
end

function dirList(path)
	local dir = {}
	fdir = io.popen("find "..path.."/* -type d")
	for line in fdir:lines() do
--		__, __, line = string.find(line,".*/(.+):")
--		if line then
			dir[#dir+1] = line
--		end
	end
	fdir:close()
	return dir
end

function fileList(path)
	local dir = {}
	local full = fullpath or false
	fdir = io.popen("ls "..path)
	for line in fdir:lines() do
		if not string.match(line,":") then
--			__, __, line = string.find(line,".*/(.+)")
			if line then
				dir[#dir+1] = line
			end
		end
	end
	fdir:close()
	return dir
end
