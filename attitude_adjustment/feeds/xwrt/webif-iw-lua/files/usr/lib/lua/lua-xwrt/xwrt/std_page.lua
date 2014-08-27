page.title = page_title
local forms = {}
__FORM.option = __FORM.option or ""
if __FORM.option == "" then
	forms = run[default]()
else
	forms = run[__FORM.option]()
end
for i = 1, #forms do
	page.content:add(forms[i])
end
page.content:add(util.table2string(__FORM,"<br>"))
page:print()
