function io.check_init(file)
  if file == nil then return 0 end
  local f = io.popen("ls /etc/rc.d/???"..file)
  for line in f:lines() do
    return 1
  end
  return 0
end

function io.exists( file )
    local f = io.open( file, "r" )
    if f then
        io.close( f )
        return true
    else
        return false
    end
end

function io.totable(filename,clean)
  local t = {}
	local data, f = load_file(filename)
	if f ~= false then
    if clean then
      for i,v in pairs(string.split(data,'\n')) do
        if string.find(string.trim(v),"#",1,true) ~= 1 then 
          if string.trim(v) ~= "" then
            t[#t+1] = v
          end
        end
      end
      return t
    end
    if data then return string.split(data,'\n') end
  end
  return t
end

