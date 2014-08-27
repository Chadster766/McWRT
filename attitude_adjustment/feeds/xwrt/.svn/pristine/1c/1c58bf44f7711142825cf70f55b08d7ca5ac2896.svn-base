var baseopacity=30
function slowhigh(which2){ imgobj=which2
browserdetect=which2.filters? "ie" : typeof which2.style.MozOpacity=="string"? "mozilla" : ""
instantset(baseopacity)
highlighting=setInterval("gradualfade(imgobj)",50)
}
function slowlow(which2){ cleartimer()
instantset(baseopacity)
}
function instantset(degree){ if (browserdetect=="mozilla")
imgobj.style.MozOpacity=degree/100
else if (browserdetect=="ie")
imgobj.filters.alpha.opacity=degree
}
function cleartimer(){ if (window.highlighting) clearInterval(highlighting)
}
function gradualfade(cur2){ if (browserdetect=="mozilla" && cur2.style.MozOpacity<1)
cur2.style.MozOpacity=Math.min(parseFloat(cur2.style.MozOpacity)+0.1, 0.99)
else if (browserdetect=="ie" && cur2.filters.alpha.opacity<100)
cur2.filters.alpha.opacity+=10
else if (window.highlighting)
clearInterval(highlighting)
}
