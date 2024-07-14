EXEC = hello

CFLAGS		:= -Os -mcpu=cortex-m3 -mthumb -I$(INSTALL_ROOT)/A2F/root/usr/include
LDFLAGS		:= -mcpu=cortex-m3 -mthumb -L$(INSTALL_ROOT)/A2F/root/usr/lib

OBJS = hello.o

CC = $(CROSS_COMPILE_APPS)gcc

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	-rm -f $(EXEC) $(EXEC).h *.gdb *.o

$(OBJS): hello.h

$(EXEC).h:
	@${RM} $@
	@echo "#ifndef _HELLO_H_" >> $@
	@echo "#define _HELLO_H_" >> $@
	@echo >> $@
	@echo "#define HELLO_STRING \"Hello, $$MCU-Linux!\n\"" >> $@
	@echo >> $@
	@echo "#endif" >> $@
