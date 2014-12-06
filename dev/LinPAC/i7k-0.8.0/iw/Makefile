SHELL = /bin/sh

CFLAGS += -c -I. -I../include
LDFLAGS =

#.SUFFIXES: .c .o

PJ   =  iw

INCS =  iw.h bsdsocket.h errexit.h
SRCS =  main.c bsdsocket.c errexit.c
OBJS =  main.o bsdsocket.o errexit.o
LIBS =  ../lib/libi7k.a

all:    $(PJ)

$(PJ):  $(OBJS)
	cd ../i7k && make
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

main.o: main.c $(INCS)
	$(CC) $(CFLAGS) main.c

bsdsocket.o: bsdsocket.c $(INCS)
	$(CC) $(CFLAGS) bsdsocket.c

errexit.o: errexit.c $(INCS)
	$(CC) $(CFLAGS) errexit.c

indent:
	indent -kr -ts4 *.c *.h

clean:
	rm -f *.o $(PJ) *~ *.tmp *.bak \#* .\#*
