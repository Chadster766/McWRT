strform = {}
local P = {}
strform = P

local print = print
local string = string
local ipairs = ipairs
local type = type
local __ENV = __ENV
local __MENU = __MENU
local tr = tr

setfenv(1, P)

local style = ""
local script = ""
local checked = ""
local str = ""

function set_style (t)
	if t.style == nil then return "" end
	if t.style ~= "" then 
		return "style=\""..t.style.."\" "
	else
		return ""
	end
end

function set_script (t)
	if t.script == nil then return "" end
	if t.script ~= "" then 
		return t.script
	else
		return ""
	end
end

function set_checked (t)
	if type(t.value) == "table" then return "" end
	if t.checked == nil then t.checked = "1" end
	if string.trim(t.value) == string.trim(t.checked) then 
		return " checked=\"checked\"" 
	else 
		return "" 
	end
end

function set_validate(t)
	local str = ""
--	local tmp = ""
--	if t.input == "text_list" then tmp = "list_" end
	if t.input == "list_add" then return str end
	if t.validate ~= "" then
		str = str .. "<input type=\"hidden\" name=\"val_str_"..t.name.."\" value=\""..t.validate.."\" />"
		str = str .. "<input type=\"hidden\" name=\"val_lbl_"..t.name.."\" value=\""..t.label.."\" />"
	end
	return str
end

function set_values (t)
	style = set_style(t)
	script = set_script(t)
	checked = set_checked(t)
	str = set_validate(t)
end

function checkbox (t)
	set_values(t)
	str = str .. "<input type=\"checkbox\" name=\""..t.name.."\" value=\"1\""..style.." "..script.." "..checked.."/>\n"
	return str
end

function text_box (t)
	set_values(t)
	str = str .. "<input type=\"text\" name=\""..t.name.."\" value=\""..t.value.."\" "..style.." "..script.." />\n"
	return str
end

function list_add (t)
	set_values(t)
	str = str .. "<table width='100%' cellspacing='0' cellspadding='0'>\n"
	str = str .. [[<tr><td width="80%"><input type="text" name="]]..t.name..[[:add" value="" ]]..style.." "..script.." /></td>\n"
	str = str .. [[<td width="20%"><input type="submit" name="]]..t.name..[[_add" value="Add" style="width:90%;float:right;"></td>]].."\n"
	str = str .."</tr>\n"
	if #t.value > 0 then
	for i=1, #t.value do
----		str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..i..[[" value="]]..t.value[i]..[[" disabled="disabled" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="]]..t.name..[[:del:]]..t.value[i]..":"..i..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		if t.disable == true then
			str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..[[:set:]]..i..[[" value="]]..t.value[i]..[[" disabled="disabled" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="]]..t.name..[[:del:]]..i..":"..t.value[i]..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		else
			str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..[[:set:]]..i..[[" value="]]..t.value[i]..[[" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="]]..t.name..[[:del:]]..i..":"..t.value[i]..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		end
	end
	end
	str = str .. "</table>\n"
	return str
end

function list (t)
	set_values(t)
	str = str .. "<table width='100%' cellspacing='0' cellspadding='0'>\n"
	local i
	for cnti=1, #t.value do
		if t.disable == true then
			str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..[[:set:]]..cnti..[[" value="]]..t.value[cnti]..[[" disabled="disabled" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="]]..t.name..[[:del:]]..cnti..":"..t.value[cnti]..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		else
			str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..[[:set:]]..cnti..[[" value="]]..t.value[cnti]..[[" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="]]..t.name..[[:del:]]..cnti..":"..t.value[cnti]..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		end
	end
	str = str .. "</table>\n"
	return str
end

function button (t)
	set_values(t)
	str = str .. "<input type=\"button\" name=\""..t.name.."\" value=\""..t.value.."\" "..style.." "..script.." />\n"
	return str
end

function file (t)
	set_values(t)
	str = str .. "<input type=\"file\" name=\""..t.name.."\" value=\""..t.value.."\" "..style.." "..script.." />\n"
	return str
end

function uci_file (t)
	set_values(t)
	if t.value == "" then
		str = str .. "<input type=\"file\" name=\"ucifile:"..t.name..":set\" value=\""..t.value.."\" style=\"width:100%;\""..script.." />\n"
	else
		str = str .. "<table width='100%' cellspacing='0' cellspadding='0'>\n"
		str = str .. [[<tr><td>]]..[[<input type="text" name="]]..t.name..[[" value="]]..t.value..[[" disabled="disabled" style="width:100%;"/>]]..[[</td><td align="right"><input type="submit" name="ucifile:]]..t.name..[[:del:]]..t.value..[[" value="Remove" style="width:90%;float:right;"></td></tr>]].."\n"
		str = str .. "</table>\n"
	end
	return str
end

function hidden (t)
	set_values(t)
	str = str .. "<input type=\"hidden\" name=\""..t.name.."\" value=\""..t.value.."\" />\n"
	return str
end

function disabled_text(t)
	set_values(t)
	str = str .. "<input type=\"text\" name=\""..t.name.."\" value=\""..t.value.."\" "..style.." "..script.." disabled=\"disabled\"/>\n"
	return str
end

function text_area(t)
	set_values(t)
	str = str .. "<TEXTAREA name=\""..t.name.."\" rows=\"6\" wrap=\"off\" "..style.." "..script.." >"..t.value.."</TEXTAREA>\n"
	return str
end

function password(t)
	set_values(t)
	str = str .. "<input type=\"password\" name=\""..t.name.."\" value=\""..t.value.."\" "..style.." "..script..">\n"
	return str
end

function select(t,tmax)
	set_values(t)
	str = str .. "<select name=\""..t.name.."\" "..style.." "..script..">\n"
	for v,op in ipairs(t.options) do
		if tmax and v > tmax then break end
		if string.trim(op.value) == string.trim(t.value) then 
			str = str .. "\t<option value=\""..op.value.."\" selected=\"selected\">"..op.label.."</option>\n"
		else
			str = str .. "\t<option value=\""..op.value.."\" >"..op.label.."</option>\n"
		end
	end
	str = str .. "</select>\n"
	return str
end

return strform