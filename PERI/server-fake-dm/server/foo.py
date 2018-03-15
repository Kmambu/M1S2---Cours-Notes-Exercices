#!/bin/env python

import time

s2f = open("/tmp/s2f_dm", "a") # opening write-only
f2s = open("/tmp/f2s_dm", "r") # opening read-only
while 1 :
    s2f.write("REQ\n")
    s2f.flush()
    time.sleep(1)
    str = f2s.readline()
    print(str)
close(s2f)
close(f2s)
