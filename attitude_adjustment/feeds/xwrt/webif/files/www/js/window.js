var dragapproved=false
var minrestore=0
var wid=0
var initialwidth,initialheight
var ie5=document.all&&document.getElementById
var ns6=document.getElementById&&!document.all
function iecompattest(){ return (!window.opera && document.compatMode && document.compatMode!="BackCompat")? document.documentElement : document.body
}
function drag_drop(e){ if (ie5&&dragapproved&&event.button==1){ document.getElementById("dwindow"+wid).style.left=tempx+event.clientX-offsetx+"px"
document.getElementById("dwindow"+wid).style.top=tempy+event.clientY-offsety+"px"
}
else if (ns6&&dragapproved){ document.getElementById("dwindow"+wid).style.left=tempx+e.clientX-offsetx+"px"
document.getElementById("dwindow"+wid).style.top=tempy+e.clientY-offsety+"px"
}
}
function initializedrag(e){ offsetx=ie5? event.clientX : e.clientX
offsety=ie5? event.clientY : e.clientY
tempx=parseInt(document.getElementById("dwindow"+wid).style.left)
tempy=parseInt(document.getElementById("dwindow"+wid).style.top)
dragapproved=true
document.getElementById("dwindow"+wid).onmousemove=drag_drop
}
function loadwindow(id,url,width,height,tX,tY){ if (!ie5&&!ns6)
window.open(url,"","width=width,height=height,scrollbars=1")
else{ document.getElementById("dwindow"+id).style.display=''
wid=id;
w =  window.innerWidth-16 || document.body.offsetWidth-20;
h =  window.innerHeight-220 || document.documentElement.clientHeight-220;
//alert(w+' '+h)
if ( tX == 0 ){ tX = (w/2)-(width/2); }
if ( tY == 0 ){ tY = (h/2)-(height); }
document.getElementById("dwindow"+id).style.width=initialwidth=width+"px"
document.getElementById("dwindow"+id).style.height=initialheight=height+"px"
document.getElementById("dwindow"+id).style.left=tX + "px"
document.getElementById("dwindow"+id).style.top=ns6? window.pageYOffset*1+tY +"px" : iecompattest().scrollTop*1+tY +"px"
}
}
function maximize(){ if (minrestore==0){ minrestore=1
document.getElementById("dwindow"+wid).style.width=ns6? window.innerWidth-20+"px" : iecompattest().clientWidth+"px"
document.getElementById("dwindow"+wid).style.height=ns6? window.innerHeight-20+"px" : iecompattest().clientHeight+"px"
}
else{ minrestore=0
document.getElementById("dwindow"+wid).style.width=initialwidth
document.getElementById("dwindow"+wid).style.height=initialheight
}
document.getElementById("dwindow"+wid).style.left=ns6? window.pageXOffset+"px" : iecompattest().scrollLeft+"px"
document.getElementById("dwindow"+wid).style.top=ns6? window.pageYOffset+"px" : iecompattest().scrollTop+"px"
}
function closeit(){ document.getElementById("dwindow"+wid).style.display="none"
}
function stopdrag(){ dragapproved=false; document.getElementById("dwindow"+wid).onmousemove=null;}
