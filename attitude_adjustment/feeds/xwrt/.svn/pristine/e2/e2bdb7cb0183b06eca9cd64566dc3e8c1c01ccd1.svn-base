--[[
--------------------------------------------------------------------------------
-- lineform.lua
--
-- Description: library of framework
--      Library to manipulate forms
--
-- Author(s) [in order of work date]:
--       Fabián Omar Franzotti .
--
-- Configuration files referenced:
--   none
--------------------------------------------------------------------------------
]]--
--[[
colClass = {}
colClass_mt = {__index = colClass}

function colClass.new(col_name, col_input, col_label, col_size, col_style)
  local self = {}
  setmetatable(self, colClass_mt)
  
  self["name"] = col_name
  self["input"]= col_input
  self["label"]= col_label
end
]]--

tbformClass = {} 
tbformClass_mt = {__index = tbformClass} 

function tbformClass.new (str_title) 
	local self = {}
	setmetatable(self,tbformClass_mt) 
	self["title"] = str_title or "Title of Form"
	self["col"] = {}
	return self 
end 

function tbformClass:Add_col(col_input, col_name, col_title, col_size, col_validate, col_style, col_script)
  if col_name == nil or col_name == "" then col_name = #self["col"]+1 end
  self["col"][#self["col"]+1] = {}
  self["col"][#self["col"]]["input"]    = col_input or "text"
  self["col"][#self["col"]]["name"]     = col_name
  self["col"][#self["col"]]["title"]    = col_title or ""
  self["col"][#self["col"]]["size"]     = col_size or ""
  self["col"][#self["col"]]["validate"] = col_validate or "string"
  self["col"][#self["col"]]["style"]    = col_style or ""
  self["col"][#self["col"]]["script"]   = col_script or ""
	self["col"][#self["col"]]["options"]  = inputoptionsClass.new()
  self["col"][col_name] = #self["col"]
end

function tbformClass:New_row()
  self[#self+1] = {}
end

function tbformClass:set_col(col_name,row_name,row_value,idx)
  local i = idx or #self
  if self[i] == nil then self[i] = {} end
	if col_name == nil or col_name == "" then return false end
--	print(i,col_name,self["col"][col_name],"<br>")
	if self[i][self["col"][col_name]] == nil then self[i][self["col"][col_name]] = {} end
  self[i][self["col"][col_name]]["var_name"] = row_name
	self[i][self["col"][col_name]]["value"]    = row_value
end

function tbformClass:print()
	print(self:tostring())
end

function tbformClass:text()
	return self:tostring()
end

function tbformClass:tostring()
  local str = self:startForm()
  str = str .. self:lines()
  str = str .. self:endForm()
  return str
end

function tbformClass:startForm()
	local str =[[
	<div class="settings">
	<h3><strong>]]..self.title..[[</strong></h3>
	<table>
	<style type='text/css'>
	<!--
	th {
    font-size: 12px;
	}
	-->
	</style>
]]
  str = str .. "<tr>"
  for i=1, #self["col"] do
    str = str .. "<th width=\""..self["col"][i]["size"].."\">"..self["col"][i]["title"].."</th>"
  end
  str = str .. "</tr>"
  return str
end

function tbformClass:lines()
  local str = ""
  for r, c in ipairs(self) do
    str = str .. "<tr>"
    for i,v in pairs(c) do
      if self["col"][i]["input"] == "label" then
        str = str .. "<td width=\""..self["col"][i]["size"].."\">"..self[r][i]["value"].."</td>"
      elseif self["col"][i]["input"] == "text" then
        str = str .. self:str_text(r,i)
      elseif self["col"][i]["input"] == "select" then
        str = str .. self:str_select(r,i)
      elseif self["col"][i]["input"] == "link" then
        str = str .. self:str_link(r,i)
      end
    end
    str = str .. "</tr>"
  end 
  return str
end

function tbformClass:endForm()
	return [[</table></div>	<div class="clearfix">&nbsp;</div>]]
end

function tbformClass:str_select(r,i)
  local str = "<td width=\""..self["col"][i]["size"].."\">"
  local style = ""
  if self["col"][i]["style"] ~= nil then style = " style=\""..self["col"][i]["style"].."\"" end
  if self["col"][i]["validate"] ~= "" then
    str = str .. "<input type=\"hidden\" name=\"val_str_"..self[r][i]["var_name"].."\" value=\""..self["col"][i]["validate"].."\" />"
    str = str .. "<input type=\"hidden\" name=\"val_lbl_"..self[r][i]["var_name"].."\" value=\""..self["col"][i]["title"].."\" />"
	end
	str = str .. "<SELECT name=\""..self[r][i]["var_name"].."\" "..style.." "..self.col[i].script..">"
	for v,op in ipairs(self.col[i].options) do
		if string.trim(op.value) == string.trim(self[r][i].value) then 
			str = str .. "<OPTION VALUE=\""..op.value.."\" SELECTED>"..op.label.."</OPTION>"
		else
			str = str .. "<OPTION VALUE=\""..op.value.."\" >"..op.label.."</OPTION>"
		end
	end
	str = str .. "</SELECT>"
  str = str .. "</td>"
	return str
end

function tbformClass:str_text(r,i)
  local str = "<td width=\""..self["col"][i]["size"].."\">"
  local style = ""
  if self["col"][i]["style"] ~= nil then style = " style=\""..self["col"][i]["style"].."\"" end
  if self["col"][i]["validate"] ~= "" then
    str = str .. "<input type=\"hidden\" name=\"val_str_"..self[r][i]["var_name"].."\" value=\""..self["col"][i]["validate"].."\" />"
    str = str .. "<input type=\"hidden\" name=\"val_lbl_"..self[r][i]["var_name"].."\" value=\""..self["col"][i]["title"].."\" />"
	end
	str = str .. "<input type=\"text\" name=\""..self[r][i]["var_name"].."\" value=\""..self[r][i]["value"].."\" "..style..self["col"][i]["script"].." />"
  str = str .. "</td>"
	return str
end

function tbformClass:str_link(r,i)
  return [[<td><a href="]]..self[r][i].value..[[">]]..self["col"][i].title..[[</a></td>]]
end
