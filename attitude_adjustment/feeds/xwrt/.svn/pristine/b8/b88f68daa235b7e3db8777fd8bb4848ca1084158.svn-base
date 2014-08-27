local hex2bin = {
	["0"] = "0000",
	["1"] = "0001",
	["2"] = "0010",
	["3"] = "0011",
	["4"] = "0100",
	["5"] = "0101",
	["6"] = "0110",
	["7"] = "0111",
	["8"] = "1000",
	["9"] = "1001",
	["a"] = "1010",
	["b"] = "1011",
	["c"] = "1100",
	["d"] = "1101",
	["e"] = "1110",
	["f"] = "1111"
	}
local bin2hex = {
	["0000"] = "0",
	["0001"] = "1",
	["0010"] = "2",
	["0011"] = "3",
	["0100"] = "4",
	["0101"] = "5",
	["0110"] = "6",
	["0111"] = "7",
	["1000"] = "8",
	["1001"] = "9",
	["1010"] = "a",
	["1011"] = "b",
	["1100"] = "c",
	["1101"] = "d",
	["1110"] = "e",
	["1111"] = "f"
	}

function string.totable(strlist)
	local t = {}
	for col in string.gmatch(strlist, "%S+") do
		col = string.gsub (col, "'", "")
		col = string.gsub (col, '"', "")
    t[#t+1] = col
	end
	return t
end

function string.trim (str)
	if str == nil then return "" end
	return string.gsub(str, "^%s*(.-)%s*$", "%1")
end

function string.split(str,sep)
	local t = {}
	local ini = 1
	local seplen = string.len(sep)
	local len = string.len(str)
	local iend= string.find(str,sep,ini,true)
	if iend == nil then iend = len+1 end
	repeat
		t[#t+1] = string.trim(string.sub(str,ini,iend-1))
		ini = iend+seplen
		iend = string.find(str,sep,ini,true)
	until iend == nil
	if ini <= len+1 then 
		t[#t+1] = string.trim(string.sub(str,ini))
	end
	return t
end

function string.capital(s)
	return s:gsub("%a", string.upper, 1)
end

function string.unescape (s)
	s = string.gsub(s, "+", " ")
	s = string.gsub(s, "%%(%x%x)", function (h)
				return string.char(tonumber(h, 16))
				end)
	return s
end

function string.escape (s)
	s = string.gsub(s, "([&=+%c])", function (c)
					return string.format("%%%02X", string.byte(c))
					end)
	s = string.gsub(s, " ", "+")
	return s
end

function string.hex2bin(s)
	return (string.gsub(s, ".", function (c)
						return hex2bin[string.lower(c)]
         end))
end

function string.bin2hex(s)
	local len = string.len(s)
	local rem = len % 4
	if (rem > 0) then
		s = string.rep("0", 4 - rem)..s
	end
	return (string.gsub(s, "....", function (c)
						return bin2hex[string.lower(c)]
         end))
end

function string.bin2dec(s)

-- s	-> binary string

local num = 0
local ex = string.len(s) - 1
local l = 0

	l = ex + 1
	for i = 1, l do
		b = string.sub(s, i, i)
		if b == "1" then
			num = num + 2^ex
		end
		ex = ex - 1
	end

	return string.format("%u", num)

end



function string.dec2bin(s, num)

-- s	-> Base10 string
-- num  -> string length to extend to

local n

	if (num == nil) then
		n = 0
	else
		n = num
	end
	
	s = string.format("%x", s)

	s = string.hex2bin(s)

	while string.len(s) < n do
		s = "0"..s
	end

	return s

end

function string.hex2dec(s)
-- s	-> hexadecimal string
	local s = string.hex2bin(s)
	return string.bin2dec(s)
end

function string.dec2hex(s)
-- s	-> Base10 string
	s = string.format("%x", s)
	return s
end
--------------------------------------------------------------------------------
function string.hexOr(v, m)
-- v	-> hex string to be masked
-- m	-> hex string mask

-- s	-> hex string as masked
-- bv	-> binary string of v
-- bm	-> binary string mask
	local bv = Hex2Bin(v)
	local bm = Hex2Bin(m)
	return string.bin2hex(string.binOr(bv,bm))
end


function string.binOr(bv, bm)
	while ((string.len(bv) %4) ~= 0) do
		bv = "0"..bv
	end
	while ((string.len(bm) %4) ~= 0) do
		bm = "0"..bm
	end
	local lbv = string.len(bv)
	local lbm = string.len(bm)
	local lt = lbv
	local i = 0
	local s = ""

	if lbm > lt then lt = lbm end
	for i = 1, lt do
		cv = string.sub(bv, i, i)
		cm = string.sub(bm, i, i)
		if i > lbv then cv="0" end
		if i > lbm then cm="0" end
		if cv == "1" then
				s = s.."1"
		elseif cm == "1" then
				s = s.."1"
		else
			s = s.."0"
		end
	end
	return s
end

function string.hexXOr(v, m)
-- v	-> hex string to be masked
-- m	-> hex string mask

-- s	-> hex string as masked

-- bv	-> binary string of v
-- bm	-> binary string mask

	local bv = string.hex2bin(v)
	local bm = string.hex2bin(m)
	return string.bin2hex(string.binXOr(bv,bm))
end

function string.binXOr(bv,bm)
	while ((string.len(bv) %4) ~= 0) do
		bv = "0"..bv
	end

	while ((string.len(bm) %4) ~= 0) do
		bm = "0"..bm
	end

	local lbv = string.len(bv)
	local lbm = string.len(bm)
	local lt = lbv
	if lbm > lbv then lt = lbm  end

	local i = 0
	local s = ""

	for i = 1, lt do
		local cv = string.sub(bv, i, i)
		local cm = string.sub(bm, i, i)
		if i > lbv then cv="0" end
		if i > lbm then cm="0" end
		if cv == "1" then
			if cm == "0" then
				s = s.."1"
			else
				s = s.."0"
			end
		elseif cm == "1" then
			if cv == "0" then
				s = s.."1"
			else
				s = s.."0"
			end
		else
			-- cv and cm == "0"
			s = s.."0"
		end
	end
	return s
end

function string.hexNot(v, m)

-- v	-> hex string to be masked
-- m	-> hex string mask

-- s	-> hex string as masked

-- bv	-> binary string of v
-- bm	-> binary string mask

	local bv = string.hex2bin(v)
	local bm = string.hex2bin(m)
	return string.bin2hex(string.binNot(bv,bm))
end

function string.binNot(bv, bm)
	while ((string.len(bv) %4) ~= 0) do
		bv = "0"..bv
	end

	while ((string.len(bm) %4) ~= 0) do
		bm = "0"..bm
	end

	local lbv = string.len(bv)
	local lbm = string.len(bm)
	local lt = lbv
	if lbm > lbv then lt = lbm  end

	local i = 0
	local s = ""
	
	for i = 1, lt do
		local cv = string.sub(bv, i, i)
		local cm = string.sub(bm, i, i)
		if i > lbv then cv="0" end
		if i > lbm then cm="0" end
		if cm == "1" then
			if cv == "1" then
				-- turn off
				s = s.."0"
			else
				-- turn on
				s = s.."1"
			end
		else
			-- leave untouched
			s = s..cv

		end
	end

	return s

end


-- these functions shift right and left, adding zeros to lost or gained bits
-- returned values are 32 bits long

-- BShRight(v, nb)
-- BShLeft(v, nb)


function BShRight(v, nb)

-- v	-> hexstring value to be shifted
-- nb	-> number of bits to shift to the right

-- s	-> binary string of v

	local s = Hex2Bin(v)

	while (string.len(s) < 32) do
		s = "0000"..s
	end

	s = string.sub(s, 1, 32 - nb)

	while (string.len(s) < 32) do
		s = "0"..s
	end

	return Bin2Hex(s)

end

function BShLeft(v, nb)

-- v	-> hexstring value to be shifted
-- nb	-> number of bits to shift to the right

-- s	-> binary string of v

	local s = Hex2Bin(v)

	while (string.len(s) < 32) do
		s = "0000"..s
	end

	s = string.sub(s, nb + 1, 32)

	while (string.len(s) < 32) do
		s = s.."0"
	end

	return Bin2Hex(s)

end
