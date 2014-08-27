
function setCookie(name, value, expires, path, domain, secure)
{
	document.cookie = name + "=" + escape(value) +
		((expires) ? "; expires=" + expires.toGMTString() : "") +
		((path) ? "; path=" + path : "") +
		((domain) ? "; domain=" + domain : "") +
		((secure) ? "; secure" : "");
}

function getCookie(name)
{
	var dc = document.cookie;
	var prefix = name + "=";
	var begin = dc.indexOf(prefix);	
	if (begin != -1)
	{		
		var end = document.cookie.indexOf(";", begin);
		if (end == -1)
		{
			end = dc.length;
		}		
		return unescape(dc.substring(begin + prefix.length, end));
	}
	else
	{
		return null;	
	}
}

function deleteCookie(name,path,domain) {
	if (getCookie(name)) {
		document.cookie = name + "=" +
			((path) ? "; path=" + path : "") +
			((domain) ? "; domain=" + domain : "") +
			"; expires=Thu, 01-Jan-70 00:00:01 GMT";
	}
}

function setcolor()
{
	var expireTime = new Date();
	OneYear = 365*24*60*60*1000;
	expireTime.setTime(expireTime.getTime()+OneYear);
	deleteCookie("webif_color", '', '');   /* old cookie name */
	deleteCookie('xwrt_color_theme', '', '');   /* previous cookie */
	setCookie('xwrt_color_theme', this.title, expireTime, '', '', '');
	colorize();
	document.close();
	window.location.href = window.location.href;
}

// find all objects of swatch class and set onclick handler
function swatch()
{
	var divs = document.getElementsByTagName("*");
	var count = 0;
	for(var i = 0; i < divs.length; i++)
	{
		if(divs[i].className.indexOf("swatch") != -1)
		{
			var colorTitle;
			switch(count)
			{
			/* count corresponds to ordering position on page */
			case 0:
				colorTitle = 'black';
				break;
			case 1:
				colorTitle = 'navyblue';
				break;
			case 2:
				colorTitle = 'blue';
				break;
			case 3:
				colorTitle = 'green';
				break;
			case 4:
				colorTitle = 'brown';
				break;
			case 5:
				colorTitle = 'white';
				break;
			default:
				colorTitle = 'blue';
				break;
			}
			divs[i].title = colorTitle;
			divs[i].onclick = setcolor;
			count++;
		}
	}
}

function colorize()
{	
	var color = getCookie('xwrt_color_theme');
	document.write('<link rel="stylesheet" type="text/css" href="');
	switch(color)
	{
	case 'black':
		document.write('/themes/active/color_black.css" />');
		break;
	case 'green':
		document.write('/themes/active/color_green.css" />');
		break;
	case 'white':
		document.write('/themes/active/color_white.css" />');
		break;
	case 'navyblue':
		document.write('/themes/active/color_navyblue.css" />');
		break;
	case 'brown':
		document.write('/themes/active/color_brown.css" />');
		break;
	case 'blue':
	default:
		document.write('/themes/active/color_blue.css" />');
		break;	
	}
}

