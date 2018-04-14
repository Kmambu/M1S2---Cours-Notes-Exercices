RPI?=26
SRC=src
APP=lcd_user
DST=debbaghi-mambu/lab3
:
CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -L$(LIB) -lgpio

KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSSDIR        = /users/enseig/franck/peri
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

all:  $(APP).x upload

$(APP).x: $(APP).c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static

upload: 
	scp -P50$(RPI) $(APP).x pi@132.227.102.36:$(DST)

clean:
	rm -f *.o *.x *~
