require("lua-xwrt.addon.string")
function handle_request(env)
	local debug = false
	local function split(str,sep)
		local t = {}
		local ini = 1
		local seplen = string.len(sep)
		local len = string.len(str)
		local iend = string.find(str,sep,ini,true)
		if iend == nil then iend = len+1 end
		repeat
			t[#t+1] = string.sub(str,ini,iend-1)
			ini = iend+seplen
			iend = string.find(str,sep,ini,true)
		until iend == nil
		if ini <= len+1 then 
			t[#t+1] = string.sub(str,ini)
		end
		return t
	end

	local function getboundary ()
		local content_type, boundary = "", ""
		if env["CONTENT_TYPE"] then
			_,_,content_type, boundary = string.find (env["CONTENT_TYPE"], "(%S+)%s*boundary%=(.-)$")
			cotent_type = content_type or ""
			if boundary then boundary = "--"..boundary
			else boundary = "" end
		end
		return content_type, boundary
	end

	function showInfo()
		uhttpd.send("HTTP/1.0 200 OK\r\n")
		uhttpd.send("Content-Type: text-plain\r\n\r\n")
	
		uhttpd.send(content_type.."\r\n")
		uhttpd.send(boundary.."\r\n")

		uhttpd.send("Request:\r\n")
		for k, v in pairs(env) do
			uhttpd.send(string.format("%35s = %s\r\n", k, tostring(v)))
		end

		uhttpd.send("\r\n\r\nHeaders:\r\n")
		for k, v in pairs(env.headers) do
			uhttpd.send(string.format("%35s = %s\r\n", k, tostring(v)))
		end
		uhttpd.send("\n\nCGI Data:\r\n\r\n---\r\n")
		for k, v in pairs(cgi) do
			uhttpd.send(string.format("%35s = %s\r\n", k, tostring(v)))
		end
		uhttpd.send("\r\n---\r\n")
		uhttpd.send("")
	end

	function getData()
		local params = {}
		if env.REQUEST_METHOD == "POST" then
			local rv, buf
			local data = ""
			repeat
				rv, buf = uhttpd.recv(4096)
				if buf then
					data = data..buf
				end
			until rv <= 0
			local bycontent = {}
			if data ~= "" then
				for k,reg in ipairs(split(data,boundary.."\r\n")) do
					local content, name, value
					string.gsub(reg, '([^%c%s:]+):%C+"(%C+)"%c+(%C+)', 
						function (k,v,z)
							content = string.lower(k)
							name = v
							value = z
						end)
					if content then
						if bycontent[content] == nil then bycontent[content] = {} end
						if name then
							bycontent[content][name] = value
							params[name] = value
						end
					end
				end
			end
			return params, bycontent
		else
			if env.QUERY_STRING then
				local str = uhttpd.urldecode(env.QUERY_STRING)
				for p in string.gmatch(str,"[^&]+") do
					for k, v in string.gfind(p,"([^=]+)=(%C*)") do
						params[k]=v
					end
				end
			end
			return params
		end
	end
		
	content_type, boundary = getboundary()
	method = env.REQUEST_METHOD
	cgi, cgibyType = getData()
	__ENV = env
	local  myenv = io.popen("env")
	for line in myenv:lines() do
		_, _, key, value = string.find(line, "([A-Z_0-9a-z]+)[%=]+(.*)")
		__ENV[key] = value
	end
	__FORM = cgi
	__HEADERS = env.headers
	cgi.REMOTE_USER = "Unkonow"
	local user, pass, fulluser = "Unknow", "invalid", ""
	if env.headers.Authorization then
		local optfile = 0
		_, _, user, pass = string.find(uhttpd.b64decode(string.sub(env.headers.Authorization,7)),"([^%:]+):(.*)")
		local huserdb = io.open("/etc/httpd.conf","r")
		for l in huserdb:lines() do
			local username, passwd, UID, GID, full_name, directory, shell
			_, _, path, username, passwd = string.find(l,"([^%:]+):([^%:]+):(.+)")
			if username and passwd then
				if string.sub(passwd,1,3) == "$p$" then
					passwddb = io.open("/etc/passwd",r)
					for p in passwddb:lines() do
						_, _, username, passwd, UID, GID, full_name, directory, shell = string.find(p,"([^%:]+):([^%:]+):([^%:]+):([^%:]+):([^%:]+):([^%:]+):(.+)")
						if username == user then
							break
						end
					end
					passwddb:close()
				end

				if username == user then
					if passwd == uhttpd.crypt(pass, passwd) then
						__REALM = {}
						__REALM["USERNAME"] = username
						__REALM["UID"] = UID
						__REALM["GID"] = GID
						__REALM["FULL_NAME"] = full_name
						__REALM["DIRECTORY"] = directory
						__REALM["SHELL"] = shell
						env.REMOTE_USER = user
					end
					break
				end
			end
		end
		huserdb:close()
		if __REALM == nil then 
			env.headers.Authorization = nil
		end
	end
	local script = env.SCRIPT_NAME..env.PATH_INFO
	local rc, err = pcall(dofile,"/www"..script)
	if not rc then
		uhttpd.send("HTTP/1.0 200 OK\r\n")
		uhttpd.send("Content-Type: text/html\r\n\r\n")
		uhttpd.send("<pre>")
		uhttpd.send("Error:\r\n\t"..err.."\r\n")
		uhttpd.send("</pre>")
	end
end
