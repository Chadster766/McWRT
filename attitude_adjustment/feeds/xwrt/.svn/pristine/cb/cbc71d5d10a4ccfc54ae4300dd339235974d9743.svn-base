--------------------------------------------------------------------------------
-- htmlpageClass.lua
--
-- Author(s) [in order of work date]:
--       Fabián Omar Franzotti
--
--------------------------------------------------------------------------------
local util = require("lua-xwrt.xwrt.util")

htmlheadtagsClass = {} 
htmlheadtagsClass_mt = {__index = htmlheadtagsClass} 

function htmlheadtagsClass.new()
	self = {}
	setmetatable(self,htmlheadtagsClass_mt)
	return self 
end

function htmlheadtagsClass:add(arg)
	if type(arg) ~= "table" then return false end
	if #arg > 0 then
		for i=1, #arg do
			table.insert(self,arg[i])
		end
	else
		table.insert(self,arg)
	end
end
	

htmlheadClass = {} 
htmlheadClass_mt = {__index = htmlheadClass} 

function htmlheadClass.new (title)
	local self = {}
	self["title"] = title or ""
	self["links"] = htmlheadtagsClass.new()
	self["metas"] = htmlheadtagsClass.new()
	self["scripts"] = htmlheadtagsClass.new()
	setmetatable(self,htmlheadClass_mt)
	return self 
end 

function htmlheadClass:text()
	local strhead = ""
	if self.title and self.title ~= "" then
		strhead = "<title>"..self.title.."</title>\n"
	end
-- ################ Los Metas #############
	if self.metas then
		for i, t in pairs(self.metas) do
			local strmeta = ""
			for k,v in pairs(t) do
				strmeta = strmeta.." "..k.."=\""..v.."\""
			end
			strhead = strhead .."<meta "..strmeta.." />\n"
		end
	end
---######### Los Links #########
	if self.links then
		for i,t in pairs(self.links) do
			local strmeta = ""
			for k,v in pairs(t) do
				strmeta = strmeta..' '..k..'="'..v..'"'
			end
			strhead = strhead .."<link "..strmeta.." />\n"
		end
	end	
---	print("######### ACA Van los SCRIPTS ##########")
	if self.scripts then
		for i, t in pairs(self.scripts) do
			nt = {}
			for k,v in pairs(t) do
				nt[string.lower(k)] = v
			end
			local strscript = "<SCRIPT "
			if nt.src then strscript = strscript ..'SRC="'..nt.src..'" ' end
			if nt.languaje then strscript = strscript ..'LANGUAJE="'..nt.languaje..'" ' end
			if nt.type then strscript = strscript ..'TYPE="'..nt.type..'" ' end
			strscript = strscript..">"
			if nt.code then  strscript = strscript.."\n"..nt.code.."\n" end
			strhead = strhead..strscript.."</SCRIPT>\n"
		end
	end
	if strhead ~= "" then
		strhead = "<HEAD>\n" .. strhead .. "</HEAD>\n"
	end
	return strhead
end

function htmlheadClass:print()
	print(self:text())
end

htmlsectionClass = {}
htmlsectionClass_mt = {__index = htmlsectionClass}

function htmlsectionClass.new(tag, id)
	local self = {}
	self["tag"] = tag
	self["id"] = id
	setmetatable(self,htmlsectionClass_mt)
	return self 
end

function htmlsectionClass:add(str)
	if str then
		self[#self+1] = str
		if type(str) == "table" then
			if str.id then 
				self[str.id] = self[#self]
			end
		end
	end
end

function htmlsectionClass:text()
	local str = ""
	for i, t in ipairs(self) do
		if type(t) == "table" then
			str = str .. t:text()
		elseif type(t) == "function" then
			str = str .. t() .."\n"
		else
			str = str .. t .. "\n"
		end
	end
	if str ~= "" then
		str = "<"..self.tag..' ID="'..self.id..'">\n'..str..'</'..self.tag..'> <!-- '..self.id.." -->\n"
	end
	return str
end

function htmlsectionClass:print()
	print(self:text())
end

htmlpageClass = {} 
htmlpageClass_mt = {__index = htmlpageClass} 

--
--	usage : page = htmlpageClass.new(title_of_page)
--	page instances
--		page.content_type		(get or set) default value come from htmlpageClass
--		page.doc_type				(get or set) default value come from htmlpageClass
--		page.html						(get or set) default value come from htmlpageClass
--		page.head						(get or set)
--		page.head.title			(get or set)
--		page.head.links			(get or set)
--		page.head.metas			(get or set)
--		page.head.scripts		(get or set)
--		page.body						(get or set)
--		page.container			(get or set)
--	page functions
--		page.head.links:add({rel="stylesheet",type="text/css",href="/path/to/file.css", media="screen", ......})
--			or you can set it page.head.links = [[<link rel="stylesheet" type="text/css" href="/themes/active/waitbox.css" media="screen" />
--	<link rel="stylesheet" type="text/css" href="/themes/active/webif.css" />
--	]]
--				but this way destroy the page.head.links:add(...) funtions.
--		also you can set page.head = [[
--<head>
--<title>System - OpenWrt Kamikaze Administrative Console</title>
--	<link rel="stylesheet" type="text/css" href="/themes/active/waitbox.css" media="screen" />
--	<link rel="stylesheet" type="text/css" href="/themes/active/webif.css" />
--	
--	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
--	<meta http-equiv="expires" content="-1" />
--	<script type="text/javascript" src="/js/styleswitcher.js"></script>
--</head>
--]]
--			but this way destroy all page.head instances and funtions to add values in dinamic way
--		page.head.metas:add({["http-equiv"] = "Content-Type", content = [[text/html; charset=UTF-8]], ......})
--		page.head.metas:add({["http-equiv"] = "Content-Type", content = [[text/html; charset=UTF-8]], ......})
--		page.container:add("add html code to container") or can use page.container:add(htmlsection(htmltag, ID_of_tag))
--		page:print()		print page
--		page:text()			return string formated html code
--
--------------------------------------------------------------------------------
function htmlpageClass.new (title)
	local self = {}
	self.title = title
	self["content_type"] = "Content-Type: text/html; charset=UTF-8\r\nPragma: no-cache\r\n\r\n"
	self["doc_type"] = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\" />\r\n\r\n"
	self["html"] = "<HTML xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"en\" xml:lang=\"en\">\r\n\r\n"
	self["head"] = htmlheadClass.new(title)
	self["body"] = "<BODY>"
	self["container"] = htmlsectionClass.new("div","container")
	setmetatable(self,htmlpageClass_mt)
	self:init()
	return self 
end 

function htmlpageClass:init()

end

function htmlpageClass:print()
	print(self:text())
end

function htmlpageClass:text()
	local str = ""
--[[
	if uhttpd == nil then
		print("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n")
	end
]]
	if self.content_type then str = self.content_type end
	if self.doc_type then str = str .. self.doc_type end
	str = str .. (self.html or "<HTML>\n")
	str = str .. self.head:text()
	str = str .. (self.body or "<BODY>") .. "\n"
	str = str .. self.container:text()
	return str .."</BODY>\n</HTML>\n"
end

