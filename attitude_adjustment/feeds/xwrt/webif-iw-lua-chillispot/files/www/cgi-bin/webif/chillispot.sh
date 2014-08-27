#!/usr/bin/lua
--------------------------------------------------------------------------------
-- chillispot.sh
-- This script is writen in LUA, the extension is ".sh" for compatibilities
-- reasons width menu system of X-Wrt
--
-- Description:
--        Administrative console to Chillispot
--
-- Author(s) [in order of work date]:
--       Fabián Omar Franzotti
--
-- Configuration files referenced:
--    hotspot
--
--------------------------------------------------------------------------------
--[[
##WEBIF:name:HotSpot:322:ChilliSpot
]]--
require ("lua-xwrt.xwrt.init")
page.title = "ChilliSpot"
cportal = require("lua-xwrt.chillispot")
local forms = {}
__FORM.option = __FORM.option or ""
if __FORM.option == "" then
	forms = cportal.service()
else
	forms = cportal[__FORM.option]()
end
for i = 1, #forms do
	page.content:add(forms[i])
end
--page.content:add(util.table2string(__FORM,"<br>"))
--page.content:add(util.table2string(uci.get_all("chillispot"),"<br>"))
page:print()

