APP=fake
OBJS=fakelib.o fake.o

%.o : %.c
	gcc -Wall -Werror -c -o $@ $< -g
$(APP) : $(OBJS)
	gcc -Wall -Werror -o $@ $^
clean :
	rm -rf *.o fake
all : $(APP)

.PHONY : clean
