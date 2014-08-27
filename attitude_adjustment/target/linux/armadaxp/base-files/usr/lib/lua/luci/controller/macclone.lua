module("luci.controller.macclone", package.seeall)

function index()
	entry({"admin", "network", "MacClone"}, cbi("macclone"), _("MAC Address Clone"), 90)
	entry({"mini", "network", "MacClone"}, cbi("macclone"), _("MAC Address Clone"), 90)
end
