#!/usr/bin/env python
import cgi, os, time,sys
form = cgi.FieldStorage()
bpVal = form.getvalue('bpval')


s2fName = '/tmp/s2f_dm'
f2sName = '/tmp/f2s_dm'
s2f = open(s2fName,'w+')
f2s = open(f2sName,'r',0)

s2f.write("r %s\n" % bpVal )
s2f.flush()
res = f2s.readline()
f2s.close()
s2f.close()
html="""
<head>
  <title>Peri Web Server</title>
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val" cols="20" placeholder ="choix Led"></input>
  <input name="ledVal" cols="20" placeholder="val Led"></input>
  <input type="submit" value="Entrer">
</form>
BP:<br/>
<form method="POST" action="bp.py">
  <input name="bpval" cols="20" placeholder="choix du BP"></input>
  <input type="submit" value="Entrer">
  read %s 
</form>
</body>
""" % (res)

print html
