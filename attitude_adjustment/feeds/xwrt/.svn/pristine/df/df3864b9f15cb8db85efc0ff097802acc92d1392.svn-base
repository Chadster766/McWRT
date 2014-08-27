#!/usr/bin/lua
require("lua-xwrt.xwrt.translator")
require("lua-xwrt.addon.uci")
tr_load()
require("lua-xwrt.addon.io")
util = require("lua-xwrt.xwrt.util")
require("set_path")
--require("common")
wwwprint = print

local reboot_list = {}
local parsers_list = {}
local old_parsers = {}
local depends_list = ""
local stop_list = {}
local file_list = {}

local exe_before = {}
local exe_after = {}

if __WWW then
  wwwprint = function (x)
    return print(x,"<br>")
  end
end
first = false

function install(pkg_list)
  if __WWW then
    require("webpkg")
    pkg.check(pkg_list)
  end
end

function exe_list(list)
  local end_line = "\n"
  local str = ""
  if __WWW then end_line = "<br>" end
  if list then
    for command, msg in util.pairsByKeys(list) do
      str = str .. msg.." "..command..end_line
      local exe_handler = io.popen(command)
      for line in exe_handler:lines() do
--        str = str.."Command Info : "..line..end_line
      end
    end
  end
  return str
end

function init_list(list)
  local tin = {}
  local tuno = {}
  local tdos = {}
  local tres = {}
  init_dir = io.popen("ls /etc/rc.d")
  for line in init_dir:lines() do
    _, _, order, file = string.find(line,"%a+(%d+)(.+)")
    tin[file]=line
  end

  for i,v in pairs(list) do
    if string.match(i,"/etc/init.d/") then
      local _, _, file, algo = string.find(i,".*/init.d/([a-zA-Z0-9._-]*)%s*(%a*)")
      if tin[file] ~= nil then
        tuno[tin[file]]=i
      else
        tdos[i]=v
      end 
    else
      tdos[i]=v
    end
  end
  
  for i,v in util.pairsByKeys(tuno) do
    tres[#tres+1]=v
  end
  
  for i,v in util.pairsByKeys(tdos) do
    tres[#tres+1]=i
  end
  local end_line = "\n"
  local spaces = "    "
  local font = ""
  local end_font ="Error!!!"..end_line
  local str = ""
  if __WWW then
		end_line = "<br>"
		spaces = "&nbsp;&nbsp;&nbsp;&nbsp;"
		font="<font color='red'><strong>"
		end_font = "</strong></font>" 
	end
  for i=1, #tres do
    io.write(tres[i]..end_line)
		local error = os.execute(tres[i].." > /tmp/"..i.."start 2> /tmp/"..i.."starterror")
    for li in io.input("/tmp/"..i.."start"):lines() do
    	io.write(spaces..li..end_line)
    end
		local myerror = io.totable("/tmp/"..i.."starterror")
		if #myerror > 1 then
			io.write(font.."<h4>"..tres[i].." Done with Error!!!</h4>"..end_line ) 
			for i,t in pairs(myerror) do
				io.write(spaces..t..end_line)
			end			
	    io.write(end_font)
		end
    os.execute("rm /tmp/"..i.."start")
    os.execute("rm /tmp/"..i.."starterror")
--[[
    exe_handler = io.popen( tres[i].." 2>&1" )
--    exe_handler = io.popen( tres[i] )
    for line in exe_handler:lines() do
			io.write(line..end_line)
      str = str..line..end_line
    end
    exe_handler:close()
]]--
		io.write(end_line)
  end
end

--function call_parser(file,parsers_list,depends_list,exe_before,exe_after,reboot_list)
function call_parser(file)
  local apply_file = uci.get(file,"system","apply") or "/usr/lib/lua/lua-xwrt/applys/"..file..".lua"
    dofile(apply_file)
    parsers_list[file] = {p = parser}
    -- Read if this package depends or need others packages to done configuration
    if parser.depends_pkgs then
      if depends_list == "" then depends_list = parser.depends_pkgs
      else depends_list = depends_list ..","..parser.depends_pkgs end
    end
		if ( parser.init_script ~= nil ) then
			exe_before[parser.init_script.." stop"] = "Stopping "

			if parser.enable == nil or tonumber(parser.enable) == 0 then
				exe_before[parser.init_script.. " disable"] = "Disabling "
			else
				exe_before[parser.init_script.. " enable"] = "Enabling "
				exe_after [parser.init_script.. " start"] = "Starting "
			end
		end
    if parser.reboot == true then
      reboot_list[#reboot_list+1] = file
    end

    if parser.exe_before then
      for command,msg in pairs(parser.exe_before) do
        exe_before[command] = msg
      end
    end

    if parser.exe_after then
      for command, msg in pairs(parser.exe_after) do
        exe_after[command] = msg
      end
    end
--[[
    wwwprint("------"..file.. "--------")
    for f, d in pairs(parsers_list) do
      wwwprint(f)
      wwwprint(d.p.name)
    end
    wwwprint("-------------------------")
]]--
end
local file_to_process = {}
for i=1, #arg do
  file_to_process[#file_to_process+1] = arg[i]
end


local handler_dir = io.popen("ls /tmp/.uci")
for file in handler_dir:lines() do
  file_to_process[#file_to_process+1] = file
end

for i=1, #file_to_process do
  local file = file_to_process[i]
  file_list[file] = ""
  local apply_file = uci.get(file,"system","apply") or "/usr/lib/lua/lua-xwrt/applys/"..file..".lua"
  if apply_file and io.exists(apply_file) == true then
--    call_parser(file,parsers_list,depends_list,exe_before,exe_after,reboot_list)
    dofile(apply_file)
    parsers_list[file] = {p = parser}
    -- Read if this package depends or need others packages to done configuration

    if parser.depends_pkgs then
      if depends_list == "" then depends_list = parser.depends_pkgs
      else depends_list = depends_list ..","..parser.depends_pkgs end
    end
		if ( parser.init_script ~= nil ) then
			exe_before[parser.init_script.." stop"] = "Stopping "
    
			if parser.enable == nil or tonumber(parser.enable) == 0 then
				exe_before[parser.init_script.. " disable"] = "Disabling "
			else
				exe_before[parser.init_script.. " enable"] = "Enabling "
				exe_after [parser.init_script.. " start"] = "Starting "
			end
    end
    if parser.reboot == true then
      reboot_list[#reboot_list+1] = file
    end
    
    if parser.exe_before then
      for command,msg in pairs(parser.exe_before) do
        exe_before[command] = msg
      end
    end

    if parser.exe_after then
      for command, msg in pairs(parser.exe_after) do
        exe_after[command] = msg
      end
    end

    if parser.call_parser then
      for u in string.gmatch(parser.call_parser,"%S+") do
        call_parser(u)
      end
    end
  elseif io.exists("/usr/lib/webif/apply-"..file) == true then
    old_parsers[file] = "/usr/lib/webif/apply-"..file
  end
end
--[[
    wwwprint("-------------------------")
    for f, d in pairs(parsers_list) do
      wwwprint(f)
      wwwprint(d.p.name)
    end
    wwwprint("-------------------------")
]]--
install(depends_list)

if __WWW then 
--	__MENU.selected = string.gsub(__SERVER.REQUEST_URI,"(.*)_changes&(.*)","%2")
--	page.title = tr("Updating config")
--	page.action_apply = ""
--	page.action_review = ""
--	page.action_clear = ""
--	page.savebutton ="<input type=\"submit\" name=\"continue\" value=\"Continue\" style=\"width:150px;\" />"
--  print(page:header())
--  wwwprint("Dependencias ",depends_list) 
end
local before_str = exe_list(exe_before)
--if before_str ~= "" then
--  wwwprint(before_str)
--end

-- After isntall all needed packages commit files with parsers
-- and execute the parsers

for f, d in pairs(parsers_list) do
--  wwwprint("File "..f)
  wwwprint("Processing "..d.p.name)
  d.p.process()
  file_list[f] = true
  wwwprint(d.p.name.." end")
  wwwprint("")
end

if #reboot_list > 0 then
  -- Commit all other files execute all other parsers and reboot
  for f, state in pairs(file_list) do
    if state ~= true then
      wwwprint("Commiting... "..f)
      wwwprint("Parsing ".. f)
    end
  end
  os.execute("reboot")
  os.exit(0)
end    
wwwprint("")
wwwprint("Please wait... Starting services...")
--print(init_list(exe_after))
init_list(exe_after)

--  local form = formClass.new("Apply...",true)
--  print (form:startFullForm())

	changes_apply=io.popen ("/usr/lib/webif/apply.sh 2>&1")
	for linea in changes_apply:lines() do
		wwwprint(trsh(linea))
	end
-- 	print (form:endForm())
	changes_apply:close()


--if __WWW then print(page:footer()) end
--os.exit(0)