old_require = require

function exists(file)
    local f = io.open( file, "r" )
    if f then
        io.close( f )
        return true
    else
        return false
    end
end

function require (str)
	local fstr = string.gsub(str,"[.]","/")
	for path in string.gmatch(package.path,"[^;]+") do
		local path = string.gsub(path,"?",fstr)
		if exists(path) then
			return old_require(str)
		end
	end
	for path in string.gmatch(package.cpath,"[^;]+") do
		local path = string.gsub(path,"?",fstr)
		if exists(path) then
			return old_require(str)
		end
	end
	return nil
end
	__WORK_STATE = {"Warning... WORK NOT DONE... Not usefull...","Warning... Work in progress...","Warning... Work Not Tested","Warning... Work in Test"}
	__WIP = 0 
	__ERROR   = {} -- __ERROR[#__ERROR][var_name], __ERROR[#__ERROR][msg]
	__TOCHECK = {} -- __TOCHECK[#__TOCHECK]
	__UCI_CMD = {} -- __UCI_CMD[#__UCI_CMD]["command"], __UCI_CMD[#__UCI_CMD_]["varname"]
	__UCI_MSG = {} -- 
	__ERROR = {}
	__ENV = {}
	__FORM = {}
	__MENU = {}
	require("lua-xwrt.xwrt.cgi_env")
	require("lua-xwrt.xwrt.validate")
	require("lua-xwrt.addon.uci")
	require("lua-xwrt.xwrt.changes_uci")
	
	uci_changed = changes_uciClass.new()
	require("lua-xwrt.xwrt.translator")
	tr_load()
	util = require("lua-xwrt.xwrt.util")
	require("lua-xwrt.xwrt.page")
	page = xwrtpageClass.new("X-Wrt Page")
	require("lua-xwrt.html.form")
--	__MENU.permission()
	if __FORM.__ACTION=="clear_changes"  then uci_changed:clear() end
	if __FORM.__ACTION=="apply_changes"  then uci_changed:apply() end
	if __FORM.__ACTION=="review_changes" then uci_changed:show() end
