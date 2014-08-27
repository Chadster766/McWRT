#!/usr/bin/lua
--------------------------------------------------------------------------------
-- vpn-opnevpn.lua
--
-- Description:
--        Administrative console to OpenVpn
--
-- Author(s) [in order of work date]:
--       Fabián Omar Franzotti
--         
-- Configuration files referenced:
--    openvpn
--
--------------------------------------------------------------------------------
--[[
##WEBIF:name:VPN:2:OpenVpn
]]--
require("lua-xwrt.xwrt.init")
local process = require("lua-xwrt.openvpn")

process.init()

page.title = "OpenVpn"
local forms ={}
__FORM.option = string.trim(__FORM.option) or ""
__FORM.name = __FORM.name or ""
ok = uci.get("openvpn", __FORM.name)
if __FORM.option == "" then
	forms = process.form_new(form)
elseif __FORM.option == "custom" then
	forms = process.form_custom(form,__FORM.name)
elseif __FORM.option == "client" then
	forms = process.form_client(form,__FORM.name)
elseif __FORM.option == "server" then
	forms = process.form_server(form,__FORM.name)
end
for k, form in ipairs(forms) do
	page.content:add(form)
end
--page.content:add(util.table2string(__FORM,"<br>"))
--page.content:add(util.table2string(__ENV,"<br>"))
page:print()
