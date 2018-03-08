.PHONY: all 
#.SECONDARY:

ifneq ($(KERNELRELEASE),)

  obj-m :=  show_sb.o

else
	
  # KERNELDIR ?= /lib/modules/$(shell uname -r)/build
  KERNELDIR ?= /home/kevin/Documents/M1S2-Cours---Notes---Exercices/PNL/linux-4.9.82
  PWD := $(shell pwd)

all :
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
	make -C $(KERNELDIR) M=$(PWD) clean

endif

