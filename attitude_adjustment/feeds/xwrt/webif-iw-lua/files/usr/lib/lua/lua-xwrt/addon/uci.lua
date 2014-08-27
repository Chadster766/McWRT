require("uci")
local www_print = print
if __WWW then
    www_print = function (x)
    return print(x,"<br>")
    end
end
-- Customized uci functions --
uci_save = uci.save
uci_commit = uci.commit
--uci_set = uci.set

uci.save = function (x)
  local ret
  if x == nil then 
    ret = uci_save()
  else
    ret = uci_save(x)
  end
  return ret
  end

uci.commit = function (x)
  local ret = false
  if x == nil then
    ret = uci_commit()
  else
    ret = uci_commit(x)
  end
  if ret == true then
--    www_print (x.." ".."Commited OK!")
    os.execute("rm /tmp/.uci/"..x.." 2> /dev/null")
  end
  return ret
end

--[[
uci.set = function(p,s,o,v)
  if p == nil then return false end
  if v == nil then
    if o == nil then
      if s == nil then
        return os.execute("uci set "..p)
      else
        return os.execute("uci set "..p.."="..s)
      end
    else
      os.execute("uci set "..p.."."..s.."="..o)
      return true
    end 
  else
    return uci_set(p,s,o,v)
  end
end
]]--

-- Added uci functions --
function uci.get_all_types(p)
  local sections = {}
  local found = false
  p = uci.get_all(p)
	if p then
  	for i, v in pairs(p) do
    	if sections[v[".type"]] == nil then sections[v[".type"]] = {} end
    	sections[v[".type"]][#sections[v[".type"]]+1] = {}
    	found = true
    	for k, o in pairs(v) do
      	sections[v[".type"]][#sections[v[".type"]]][k] = o
    	end
  	end
  end
	if found == true then
    return sections
  else
    return nil
  end
end

function uci.get_type(p,s)
  local sections = {}
  local found = false
  if string.find(p,".") > 0 and s == nil then
    p,s = unpack(string.split(p,"."))
  end
  p = uci.get_all(p)
  if p then
  	for i, v in pairs(p) do
    	if v[".type"] == s then
      	sections[#sections+1] = {}
      	found = true
      	for k, o in pairs(v) do
        	sections[#sections][k] = o
      	end
    	end
  	end
	end
  if found == true then
    return sections
  else
    return nil
  end
end

function uci.get_section_name ( p, t, args )
	local sections = uci.get_type(p,t)
	local names = {}
	for i=1, #sections do
		local ok = true
		for k,v in pairs(args) do
			if sections[i][k] ~= v then ok = false end
		end
		if ok == true then
			names[#names+1]=sections[i][".name"]
		end
	end
	if #names == 0 then return nil
	elseif #names == 1 then return names[1]
	else return "Error fond "..#names.."sections with that criterion search" end
end

function uci.get_section(p,s)
  local t = uci.get_all(p)
  return t[s]
end

function uci.updated()
  local mycount = 0
	local BUFSIZE = 2^13     -- 8K
	assert(os.execute("mkdir /tmp/.uci > /dev/null 2>&1"))
	local filelist = assert(io.popen("ls /tmp/.uci")) 
	for filename in filelist:lines() do
		local f = io.input("/tmp/.uci/"..filename)   -- open input file
		while true do
			local lines, rest = f:read(BUFSIZE, "*line")
			if not lines then break end
			if rest then lines = lines .. rest .. '\n' end
			for li in string.gmatch(lines,"[^\n]+") do
        mycount = mycount + 1
			end
		end
	end
  return mycount
end

function uci.check_set(p,s,o,v)
  local r
  if p == nil then return nil end
  if p and s and o and v then
    r = uci.get(p,s,o)  
    if r == nil then
      uci.set(p,s,o,v)
      r = v
    end
    return r
  elseif p and s and o and v == nil then
    if uci.get(p,s) == nil then 
      return uci.set(p,s,o)
    end
  end
  return nil
end

function uci.isdiff_set(p,s,o,v)
  local r
  if p and s and o and v then
    r = uci.get(p,s,o)  
    if r ~= v then
      uci.set(p,s,o,v)
      r = v
    end
    return r
  end
  return nil
end
