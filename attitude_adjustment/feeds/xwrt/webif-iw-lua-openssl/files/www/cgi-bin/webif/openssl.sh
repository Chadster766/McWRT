#!/usr/bin/lua
--------------------------------------------------------------------------------
-- openssl.sh
--
-- Description:
--        Administrative console to OpenSSL
--
-- Author(s) [in order of work date]:
--       Fabián Omar Franzotti
--         
-- Configuration files referenced:
--    OpenSSL
--
--------------------------------------------------------------------------------
--[[
##WEBIF:name:VPN:1:OpenSSL
]]
require("lua-xwrt.xwrt.init")
process = require("lua-xwrt.openssl")
process.init()
page.title = "OpenSSL"
local forms ={}
__FORM.option = string.trim(__FORM.option) or ""
__FORM.name = __FORM.name or ""
if __FORM.option == "new" then
	forms = process.form_new(form)
elseif __FORM.option == "entity" then
	forms = process.form_entity(form,__FORM.name)
elseif __FORM.option == "client" then
	forms = process.form_client(form,__FORM.name)
elseif __FORM.option == "server" then
	forms = process.form_server(form,__FORM.name)
else
	forms = process.form_wellcome()
end
for k, form in ipairs(forms) do
	page.content:add(form)
end
--page.content:add(util.table2string(__FORM,"<br>"))
page:print()

