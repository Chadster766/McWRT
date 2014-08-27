/*
 * CodePress - Real Time Syntax Highlighting Editor written in JavaScript - http://codepress.fermads.net/
 * 
 * Copyright (C) 2006 Fernando M.A.d.S. <fermads@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the 
 * GNU Lesser General Public License as published by the Free Software Foundation.
 * 
 * Read the full licence: http://www.opensource.org/licenses/lgpl-license.php
 */

CodePress = {
	range : null,
	language : null,
	scrolling : false,
		
	// set initial vars and start sh
	initialize : function() {
		if(typeof(editor)=='undefined'&&!arguments[0]) return;
		this.detect();
		chars = '|13|32|191|57|48|187|188|'; // charcodes that trigger syntax highlighting
		cc = '&shy;'; // control char
		if(browser.ff) {
			editor = document.getElementById('ffedt');
			document.designMode = 'on';
			document.addEventListener('keydown', this.keyHandler, true);
			window.addEventListener('scroll', function() { if(!CodePress.scrolling) CodePress.syntaxHighlight('scroll'); }, false);
			//document.body.focus();
		}
		else if(browser.ie) {
			editor = document.getElementById('ieedt');
			editor.contentEditable = 'true';
			document.onkeydown = this.keyHandler;
			window.onscroll = function() { if(!CodePress.scrolling) CodePress.syntaxHighlight('scroll'); }
		}
		else {
			// TODO: textarea without syntax highlighting for non supported browsers
			alert('your browser is not supported at the moment');
			return;
		}
		this.syntaxHighlight('init');
		setTimeout(function() { window.scroll(0,0) },50);
	},

	// detect browser, for now IE and FF
	detect : function() {
		browser = { ie:false, ff:false };
		if(navigator.appName.indexOf("Microsoft") != -1) browser.ie = true;
		else if (navigator.appName == "Netscape") browser.ff = true;
	},

	// treat key bindings
	keyHandler : function(evt) {
		evt = (evt) ? evt : (window.event) ? event : null;
	  	if(evt) {
	    	charCode = (evt.charCode) ? evt.charCode : ((evt.keyCode) ? evt.keyCode : ((evt.which) ? evt.which : 0));
		    if((chars.indexOf('|'+charCode+'|')!=-1) && (!evt.ctrlKey && !evt.altKey)) { // syntax highlighting
			 	CodePress.syntaxHighlight('generic');
			}
			else if(charCode==46||charCode==8) { // save to history when delete or backspace pressed
			 	CodePress.actions.history[CodePress.actions.next()] = editor.innerHTML;
			}
			else if((charCode==90||charCode==89) && evt.ctrlKey) { // undo and redo
				(charCode==89||evt.shiftKey) ? CodePress.actions.redo() : CodePress.actions.undo() ;
				evt.returnValue = false;
				if(browser.ff)evt.preventDefault();
			}
			else if(charCode==86 && evt.ctrlKey)  { // paste
				// TODO: pasted text should be parsed and highlighted
			}
		}
	},

	// put cursor back to its original position after every parsing
	findString : function() {
		if(browser.ff) {
			if(self.find(cc))
				window.getSelection().getRangeAt(0).deleteContents();
		}
		else if(browser.ie) {
		    range = self.document.body.createTextRange();
			if(range.findText(cc)){
				range.select();
				range.text = '';
			}
		}
	},
	
	// split big files, highlighting parts of it
	split : function(code,flag) {
		if(flag=='scroll') {
			this.scrolling = true;
			return code;
		}
		else {
			this.scrolling = false;
			mid = code.indexOf("&amp;shy;"); 
			if(mid-2000<0) {ini=0;end=4000;}
			else if(mid+2000>code.length) {ini=code.length-4000;end=code.length;}
			else {ini=mid-2000;end=mid+2000;}
			code = code.substring(ini,end);
			if(browser.ff) return code;
			else return code.substring(code.indexOf('<P>'),code.lastIndexOf('</P>')+4);
		}
	},

	
	// syntax highlighting parser
	syntaxHighlight : function(flag) {
		if(browser.ff) {
			if(flag!='init') window.getSelection().getRangeAt(0).insertNode(document.createTextNode(cc));
			o = editor.innerHTML;
			o = o.replace(/<br>/g,'\n');
			o = o.replace(/<.*?>/g,'');
			x = z = this.split(o,flag);
			x = x.replace(/\n/g,'<br>');
		}
		else if(browser.ie) {
			if(flag!='init') document.selection.createRange().text = cc;
			o = editor.innerHTML;
			o = o.replace(/<P>/g,'\n');
			o = o.replace(/<\/P>/g,'\r');
			o = o.replace(/<.*?>/g,'');
			o = '<PRE><P>'+o+'</P></PRE>';
			o = o.replace(/\n/g,'<P>');
			o = o.replace(/\r/g,'<\/P>');
			o = o.replace(/<P>(<P>)+/,'<P>');
			o = o.replace(/<\/P>(<\/P>)+/,'</P>');
			o = o.replace(/<P><\/P>/g,'<P>&nbsp;<\/P>');
			x = z = this.split(o,flag);
		}

		for(i=0;i<syntax.length;i++) 
			x = x.replace(syntax[i],syntax[i+1]);

		editor.innerHTML = this.actions.history[this.actions.next()] = (flag=='scroll') ? x : o.replace(z,x);

		if(flag!='init') this.findString();
	},

	// undo and redo methods
	actions : {
		pos : -1, // actual history position
		history : [], // history vector
		
		undo : function() {
			if(editor.innerHTML.indexOf(cc)==-1){
				if(browser.ff) window.getSelection().getRangeAt(0).insertNode(document.createTextNode(cc));
				else document.selection.createRange().text = cc;
			 	this.history[this.pos] = editor.innerHTML;
			}
			this.pos--;
			if(typeof(this.history[this.pos])=='undefined') this.pos++;
			editor.innerHTML = this.history[this.pos];
			CodePress.findString();
		},
		
		redo : function() {
			this.pos++;
			if(typeof(this.history[this.pos])=='undefined') this.pos--;
			editor.innerHTML = this.history[this.pos];
			CodePress.findString();
		},
		
		next : function() { // get next vector position and clean old ones
			if(this.pos>20) this.history[this.pos-21] = undefined;
			return ++this.pos;
		}
	},	
	
	// transform syntax highlighted code to original code
	getCode : function() {
		code = editor.innerHTML;
//		code = code.replace(/<pre>(<p>)*|(<\/p>)*<\/pre>/gi,'');
		code = code.replace(/<br>/gi,'\n');
		code = code.replace(/<\/p>/gi,'\r');
		code = code.replace(/<p>/gi,'\n');
		code = code.replace(/&nbsp;/gi,'');
		code = code.replace(/&shy;/gi,'\xad');
		code = code.replace(/<.*?>/g,'');
		code = code.replace(/&lt;/g,'<');
		code = code.replace(/&gt;/g,'>');
		code = code.replace(/&amp;/gi,'&');
		return code;
	},

	// put some code inside editor
	// you can pass parameters like (language,code) or just textarea object id
	setCode : function() {
		if(typeof(arguments[1])=='undefined') {
			language = top.document.getElementById(arguments[0]).lang.toLowerCase();
			code = top.document.getElementById(arguments[0]).value;
		} 
		else {
			language = arguments[0];
			code = arguments[1];
		}
		document.designMode = 'off';
	   	head = document.getElementsByTagName('head')[0];
	   	script = document.createElement('script');
	   	script.type = 'text/javascript';
	   	script.src = 'languages/codepress-'+language+'.js';
		head.appendChild(script)
		document.getElementById('cp-lang-style').href = 'languages/codepress-'+language+'.css';
		code = code.replace(/&shy;|&amp;shy;/gi,'\xad');
		code = code.replace(/&/gi,'&amp;');		
       	code = code.replace(/</g,'&lt;');
        code = code.replace(/>/g,'&gt;');
		editor.innerHTML = "<pre>"+code+"</pre>";
		this.language = language;
	}
}

onload = function() {
	cpWindow = top.document.getElementById('codepress');
	if(cpWindow!=null) {
		cpWindow.style.border = '1px solid gray';
		cpWindow.style.frameBorder = '0';
	}
	
	top.CodePress = CodePress;
	CodePress.initialize('new');
	
	cpOnload = top.document.getElementById('codepress-onload');
	cpOndemand = top.document.getElementById('codepress-ondemand');
	
	if(cpOnload!=null) {
		cpOnload.style.display = 'none';
		cpOnload.id = 'codepress-loaded';
		CodePress.setCode('codepress-loaded');
	}
	if(cpOndemand!=null) cpOndemand.style.display = 'none';
}
