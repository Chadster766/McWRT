function value(name)
{
	var item = document.getElementById(name);
	return (item ? item.value : "");
}
function set_value(name, value)
{
	var item = document.getElementById(name);
	if (item) item.value = value;
}
function isset(name, val)
{
	return (value(name) == val);
}
function checked(name)
{
	var item = document.getElementById(name);
	return ((item) && item.checked);
}
function hide(name)
{
	var item = document.getElementById(name);
	if (item) 
		item.style.display = 'none';
}
function show(name)
{
	var item = document.getElementById(name);
	if (item)
		item.style.display = '';
}
function set_visible(name, value)
{
	if (value)
		show(name)
	else
		hide(name)
}
function processKey(e)
{
	var key;

	if(window.event)
		key = window.event.keyCode; //IE
	else if(e.which)
		key = e.which; //Netscape/Firefox/Opera
	else
		return true;

	if (key == 13) {
		document.getElementById("savebutton").click();
		return false;
	}
	else
		return true;
}
