
-- compare two values and retur true or false
-- is the same as if but this is dynamic
-- op can be "==, ~=, >,<, >=, <=, #
function check_value(val,op,comp)
	if op == "=" then op = "=="
	elseif op == "#" then op = "~=" end
	if tonumber(val) and tonumber(comp) then
		return loadstring("if "..val..op..comp.." then return true else return false end")()
	else
		local str = string.format("if \"%s\" %s \"%s\" then return true else return false end",string.trim(val),op,comp)
		return loadstring(str)()
	end
end

-- make automatic validation
function validate_post()
	for i, v in ipairs(__TOCHECK) do
		local error = validate(__FORM["val_lbl_"..v],__FORM[v],__FORM["val_str_"..v])
		if error ~=nil then __ERROR[#__ERROR+1] = error end
	end
end

function validate(str_name,str_value,s,str_var)
	local i = 0
	local list = {}
	local option = {}
	option[1] = {}
	option[2] = {}
	option[3] = {}
	local result
	local ret = {}

	s = string.trim(s)
	str_name = string.trim(str_name)
	str_value = string.trim(str_value)
	if s == nil then 
		ret["var_name"] = string.capital(tr("validate_no_s#validate strring"))
		ret["msg"] = string.capital(tr("Error!!! Type param is missing"))
		return ret
	end
	if str_name == nil then 
		ret["var_name"] = string.capital(tr("validate_no_s#validate strring"))
		ret["msg"] = string.capital(tr("ERROR!!! Name param is missing"))
		return ret
	end
	if str_value == nil then
		ret["var_name"] = string.capital(tr("validate_no_s#validate strring"))
		ret["msg"] = string.capital(tr("ERROR!!! Value param is missing")) 
		return ret
	end
	local str_type = string.trim(string.match(string.trim(s),"%a+%s-"))
	local aux_type
	local bool_type = false
	local str_error = ""
	local str_or_error = ""
	local slen = string.len(str_type)
	s = string.sub(s,slen+1)
	if str_type == "string" then
		bool_type = true
	elseif str_type == "int" or str_type == "number" then
		bool_type = true 
		if str_value ~= nil and str_value ~="" and not tonumber(str_value) then 
			bool_type = false
			result = "false"
			str_error = tr("validate_number#must be a number")
		end
	elseif str_type == "email" then 
		bool_type = true
		if str_value ~= nil and str_value ~= "" and not string.match(str_value,"[A-Za-z0-9%.%%%+%-]+@[A-Za-z0-9%.%%%+%-]+%.%w%w%w?%w?") then
			bool_type = false
			result = "false"
			str_error = tr("validate_no_email#invalid e-mail address")
		end
	elseif str_type == "mac" then 
		bool_type = true
		if str_value ~= nil and str_value ~= "" and not string.match(str_value,"%x%x[:]%x%x[:]%x%x[:]%x%x[:]%x%x[:]%x%x") then
			str_error = tr("invalid mac address")
			result = "false"
			bool_type = false 
		end
	elseif str_type == "hostname" then 
		bool_type = true
		if str_value ~= nil and str_value ~= "" then
			s = s .."nospaces,nohostchar"
		end
	else
		result = "false"
		bool_type = false
		str_error = str_type .." ".. tr("validate_untype#undefined type")
	end 
	if string.len(s) == 0 and bool_type == true then result = "true" end
	if s ~= nil and string.len(s) > 0 and bool_type == true then
		if string.sub(s,1,1)=="," then s = string.sub(s,2) end
		for l in string.gmatch(s,"%b{}") do
			i = i + 1
			list[#list+1] = string.gsub(l,"[{}]","")
			s = string.gsub(s,l,"["..i.."]")
		end
		s = string.gsub(s,";",",")
		s = string.gsub(s," ","")

		for param in string.gmatch(s,"[^,]+") do
			if param ~= nil and param ~= "" then
				param = string.gsub(param,"([=#<>])"," %1 ")
				param = string.gsub(param,"([=#<>])%s*([=#<>])", "%1%2")
				result = ""
				local str_error
				local t = {}
				local count = 0
				local con =""
				for v in string.gmatch(string.trim(param),"%S+") do
					t[#t+1] = v
				end 
				if str_value == nil or str_value == "" then 
					result = true
					con = 1
					if t[1] == "required" then
						result = false
						str_error = tr("validate_no_value#no value entered")
					end
					option[con][#option[con]+1] = {}
					option[con][#option[con]]["result"] = tostring(result)
					option[con][#option[con]]["error"] = str_error
				else
					if #t == 2 then
						if string.find(t[2],"%[") == nil then
							if t[1] == "==" or t[1] == "=" then 
								con = 2
								result = check_value(str_value,"==",t[2])
								str_error = "=" .. t[2]
							else 
								con = 1
								result = check_value(str_value,t[1],t[2])
								str_error = tr("validate_must_be#value must be").." "..t[1].." "..t[2]
							end
--							if str_value == nil or str_value == "" then result = true end
						else
							if t[1] == "=" or t[1] == "==" then con = 2 else con = 1 end
--							if str_value == nil or str_value == "" then con = 3 end
							local idx = tonumber(string.match(t[2],"%d"))
							for v in string.gmatch(list[idx],"[^,]+") do
								result = check_value(str_value, t[1], v)
								option[con][#option[con]+1] = {}
								option[con][#option[con]]["result"] = tostring(result)
								option[con][#option[con]]["error"] = t[1]..v
							end
						end
					else
--						if str_value ~= nil and str_value ~= "" then
						if t[1] == "required" then
						  result = true
						elseif t[1] == "len" then 
							result = check_value(string.len(str_value), t[2], t[3])
							str_error = tr("validate_len#lenght must be").." " .. t[2] .." ".. t[3]
						elseif t[1] == "nospaces" then
							_, count = string.gsub(str_value, " ", " ")
							if count > 0 then result = false else result = true end
							str_error =  tr("validate_have#have").." "..tr("validate_spaces#invalid spaces")
						elseif t[1] == "nodots" then
							_, count = string.gsub(str_value, "%.", ".")
							if count > 0 then result = false else result = true end
							str_error = tr("validate_have#have").." "..tr("validate_dots#invalid dots")
						elseif t[1] == "nohostchar" then
--							local invalidChars = string.gsub(str_value, "([a-zA-Z0-9%-]+)(%W+)([a-zA-Z0-9]+)","%2 ")
							local invalidChars = ""
							for c in string.gmatch(str_value, "[^a-zA-Z0-9%-]") do
								invalidChars = invalidChars .. c
							end
							if invalidChars == "" then result = true
							else
								invalidChars = string.gsub(invalidChars,"(.)","%1 ")
								result = false 
								str_error = tr("validate_have#have").." "..tr("validate_invalidChars#invalid chars").." <strong>\""..invalidChars.."\"</strong>"
							end
						elseif t[1] == "noalfanumeric" then
							local invalidChars = string.gsub(str_value, "([a-zA-Z0-9])(%W+)([a-zA-Z0-9])","%2")
							if invalidChars then 
								result = false 
								str_error = tr("validate_have#have").." "..tr("validate_invalidChars#invalid chars").." \""..invalidChars.."\""
							else result = true end
						else 
							str_error = t[1].." "..tr("validate_unknow_option#unknow option").." "..tr("validate_dots#invalid dots")
							result = false
						end
						con = 1
					end		

					if not(#t == 2 and string.find(t[2],"%[") ~= nil) then
						option[con][#option[con]+1] = {}
						option[con][#option[con]]["result"] = tostring(result)
						option[con][#option[con]]["error"] = str_error
					end
 				end
			end
		end

		result = ""
		str_error = ""
		for idx , v in ipairs(option[1]) do
			if result == "" then result = v["result"]
			else result = result.." and "..v["result"] end
			if v["result"] == "false" then 
				if str_error == "" then str_error = tr(v["error"])
				else str_error = str_error.." "..tr("validate_and").." "..tr(v["error"]) end
			end
		end
		str_or_error = ""
		for idx , v in ipairs(option[2]) do
			if result == "" then result = v["result"]
			else result = result.." or "..v["result"] end
			if v["result"] == "false" then 
				if str_or_error == "" then str_or_error = tr(v["error"])
				else str_or_error = str_or_error.." "..tr("validate_or").." "..tr(v["error"]) end
			end
		end
		if result == "" and #option[3] > 0 then
--			print (#option[3],"<br>")
			result = "true"
		end
	end
	

	if str_error ~= "" then str_error = string.capital(str_error).."." end
	if str_or_error ~= "" then str_or_error = string.capital(tr("validate_value_can_be#value can be").." "..str_or_error).."." end
	
--	str_error = str_name.." = "..str_value..", "..tr("validate_invalid_value#invalid value")..". "..str_error.." "..str_or_error
--	str_error = str_value..", "..tr("validate_invalid_value#invalid value")..". "..str_error.." "..str_or_error
	str_error = str_error.." "..str_or_error
	result = loadstring("if "..result.." then return true else return false end")()
	if result == true then return nil
	else
		ret["var_name"] = str_name
		ret["msg"] = str_error
		ret["var"] = str_var
	end
	return ret
end
