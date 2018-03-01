############################### COMPILATION TOOLS ##############################
GIET_SYS_PATH=/users/enseig/alain/giet_2011/sys
GIET_APP_PATH=/users/enseig/alain/giet_2011/app
AS=/opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-as
CC=/opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-gcc
LD=/opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-ld
DU=/opt/gcc-cross-mipsel/4.3.3/bin/mipsel-unknown-elf-objdump

############################### COMPILATION FLAGS ##############################
APP_FLAGS=-Wall -mno-gpopt -ffreestanding -mips32 -I$(GIET_APP_PATH) -I. -c
SYS_FLAGS=-Wall -mno-gpopt -ffreestanding -mips32 -I$(GIET_SYS_PATH) -I. -c
AS_FLAGS=-g -mips32

##################################### FILES ####################################
GIET_SYS_OBJS 	=	drivers.o\
					common.o\
					ctx_handler.o\
					irq_handler.o\
					sys_handler.o\
					exc_handler.o
SYS_OBJS		=	reset.o giet.o

GIET_APP_OBJS	=	stdio.o
APP_OBJS		=	main.o

################################################################################
all 				: sys.bin app.bin

############################## WILDCARD RULES ##################################
$(GIET_SYS_OBJS)	: %.o : $(GIET_SYS_PATH)/%.c
	$(CC) $(SYS_FLAGS) -o  $@ $<

$(GIET_APP_OBJS)	: %.o : $(GIET_APP_PATH)/%.c
	$(CC) $(APP_FLAGS) -o  $@ $<

############################## SPECIFIC RULES ##################################
reset.o				: reset.s
	$(AS) $(AS_FLAGS) -o $@ $<

giet.o				: $(GIET_SYS_PATH)/giet.s
	$(AS) $(AS_FLAGS) -o $@ $<

main.o				: main.c
	$(CC) $(APP_FLAGS) -o  main.o main.c

############################  MAIN TARGET RULES ################################
sys.bin				: $(SYS_OBJS) $(GIET_SYS_OBJS)
	$(LD) -o $@ -T sys.ld $^

app.bin				: $(GIET_APP_OBJS) $(APP_OBJS)
	$(LD) -o $@ -T app.ld $^

###################### META-RULES (clean,dump files) ###########################
sys_dump			: sys.bin
	$(DU) -D sys.bin > sys.bin.txt

app_dump			: app.bin
	$(DU) -D app.bin > app.bin.txt

clean				:
	rm -rf app.bin sys.bin *.o *.bin.txt

.PHONY				: clean
