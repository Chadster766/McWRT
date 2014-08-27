for u, v in pairs(__FORM) do
	if string.match(u,"val_str_") then __TOCHECK[#__TOCHECK+1]=string.sub(u,9) end
	local proc, uci_var, uci_cmd, idx, uci_val = unpack(string.split(u,":"))
	if uci_var ~= nil and uci_cmd ~= nil then
		if proc == "service" then -- Este va a general
			os.execute("/etc/init.d/"..uci_var.." "..uci_cmd)
			if uci_cmd == "close" then
				os.execute("killall "..uci_var)
			end
			os.execute("sleep 3")
		end
	end
end
