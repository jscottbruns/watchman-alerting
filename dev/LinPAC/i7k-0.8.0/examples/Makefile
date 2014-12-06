PJ = send_read send_read2 send_read3 dio
SHELL = /bin/sh


CFLAGS =  -I. -I../include
LDFLAGS =

ifeq ($(LinPAC), ARM)
        CROSS_COMPILE   = arm-linux-
        LIBS = ../lib/libi7k_arm.a
else
        CROSS_COMPILE   =
        LIBS = ../lib/libi7k.a
endif

AS              = $(CROSS_COMPILE)as
LD              = $(CROSS_COMPILE)ld
CC              = $(CROSS_COMPILE)gcc
CPP             = $(CC) -E
AR              = $(CROSS_COMPILE)ar
NM              = $(CROSS_COMPILE)nm
STRIP           = $(CROSS_COMPILE)strip
OBJCOPY         = $(CROSS_COMPILE)objcopy
OBJDUMP         = $(CROSS_COMPILE)objdump

#ifeq ($(Device), I7565H1H2)
ifdef Device

ifeq ($(Device), I7565H1H2)
all:    send_read send_read2
else
all:    send_read3
endif

else
all:    send_read send_read2 send_read3 dio 
endif

dio: dio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ dio.o $(LIBS)
	rm -f dio.o

send_read: send_read.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ send_read.o $(LIBS)
	rm -f send_read.o

send_read2: send_read2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ send_read2.o $(LIBS)
	rm -f send_read2.o

send_read3: send_read3.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ send_read3.o $(LIBS)
	rm -f send_read3.o

indent:
	indent -kr -ts4 *.c

clean:
	rm -f *.o $(PJ) *~ *.tmp *.bak \#* .\#*
