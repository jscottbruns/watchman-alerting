#SHELL = /bin/sh

LIBS = ../lib/libi8k.a

CFLAGS =  -I. -I../include
LDFLAGS = -lm

#GTKCFLAGS =  -I. -I../include `PKG_CONFIG_PATH=/opt/lib/pkgconfig pkg-config --cflags --libs gtk+-2.0 gthread-2.0`

CROSS_COMPILE 	= arm-linux-

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump




all:     helloworld getlist read_sn echosvr setport getport port uart send_receive eeprom timer timer2 i7kdio i7kaio getexdi setexdo getexai setexao 
all:     getsendreceive getsendreceive-bin setsend getreceive sram wdt wdt_safe_value getdo_rb setdo_bw
all:     blocking non_blocking demo8080 demo8050 demo8090 demo8091 demo8172 i8kdio i8kaio i8kaio2 i8kaio_8k i8kdio_8k int i87kdio setdo setao getdi getai sio_port 
all:     demo1 demo2 i87kaio i87kdio i87kaio_87k i87kdio_87k 
#all:    send_receive send_read dio


helloworld: ./common/helloworld.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/helloworld.o $(LIBS)
	rm -f ./common/helloworld.o

getlist: ./common/getlist.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getlist.o $(LIBS)
	rm -f ./common/getlist.o

read_sn: ./common/read_sn.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/read_sn.o $(LIBS)
	rm -f ./common/read_sn.o

send_receive: ./common/send_receive.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/send_receive.o $(LIBS)
	rm -f ./common/send_receive.o

getsendreceive: ./common/getsendreceive.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getsendreceive.o $(LIBS)
	rm -f ./common/getsendreceive.o

getsendreceive-bin: ./common/getsendreceive-bin.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getsendreceive-bin.o $(LIBS)
	rm -f ./common/getsendreceive-bin.o

setsend: ./common/setsend.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/setsend.o $(LIBS)
	rm -f ./common/setsend.o

getreceive: ./common/getreceive.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getreceive.o $(LIBS)
	rm -f ./common/getreceive.o

eeprom: ./common/eeprom.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/eeprom.o $(LIBS)
	rm -f ./common/eeprom.o

timer: ./common/timer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/timer.o $(LIBS)
	rm -f ./common/timer.o

timer2: ./common/timer2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/timer2.o $(LIBS)
	rm -f ./common/timer2.o

uart: ./common/uart.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/uart.o $(LIBS)
	rm -f ./common/uart.o

port: ./common/port.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/port.o $(LIBS)
	rm -f ./common/port.o

setport: ./common/setport.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/setport.o $(LIBS)
	rm -f ./common/setport.o

getport: ./common/getport.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getport.o $(LIBS)
	rm -f ./common/getport.o

echosvr: ./common/echosvr.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/echosvr.o $(LIBS)
	rm -f ./common/echosvr.o

getexdi: ./common/getexdi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getexdi.o $(LIBS)
	rm -f ./common/getexdi.o

setexdo: ./common/setexdo.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/setexdo.o $(LIBS)
	rm -f ./common/setexdo.o

getexai: ./common/getexai.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getexai.o $(LIBS)
	rm -f ./common/getexai.o

setexao: ./common/setexao.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/setexao.o $(LIBS)
	rm -f ./common/setexao.o

wdt: ./common/wdt.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/wdt.o $(LIBS)
	rm -f ./common/wdt.o

sram: ./common/sram.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/sram.o $(LIBS)
	rm -f ./common/sram.o

wdt_safe_value: ./common/wdt_safe_value.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/wdt_safe_value.o $(LIBS)
	rm -f ./common/wdt_safe_value.o

getdo_rb: ./common/getdo_rb.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/getdo_rb.o $(LIBS)
	rm -f ./common/getdo_rb.o

setdo_bw: ./common/setdo_bw.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./common/$@ ./common/setdo_bw.o $(LIBS)
	rm -f ./common/setdo_bw.o

i7kdio: ./i7k/i7kdio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i7k/$@ ./i7k/i7kdio.o $(LIBS)
	rm -f ./i7k/i7kdio.o

i7kaio: ./i7k/i7kaio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i7k/$@ ./i7k/i7kaio.o $(LIBS)
	rm -f ./i7k/i7kaio.o

i87kdio: ./i87k/i87kdio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i87k/$@ ./i87k/i87kdio.o $(LIBS)
	rm -f ./i87k/i87kdio.o

i87kaio: ./i87k/i87kaio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i87k/$@ ./i87k/i87kaio.o $(LIBS)
	rm -f ./i87k/i87kaio.o

i87kdio_87k: ./i87k/i87kdio_87k.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i87k/$@ ./i87k/i87kdio_87k.o $(LIBS)
	rm -f ./i87k/i87kdio_87k.o

i87kaio_87k: ./i87k/i87kaio_87k.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i87k/$@ ./i87k/i87kaio_87k.o $(LIBS)
	rm -f ./i87k/i87kaio_87k.o

i8kdio: ./i8k/i8kdio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/i8kdio.o $(LIBS)
	rm -f ./i8k/i8kdio.o

i8kaio: ./i8k/i8kaio.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/i8kaio.o $(LIBS)
	rm -f ./i8k/i8kaio.o
	
i8kaio2: ./i8k/i8kaio2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/i8kaio2.o $(LIBS)
	rm -f ./i8k/i8kaio2.o	

i8kdio_8k: ./i8k/i8kdio_8k.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/i8kdio_8k.o $(LIBS)
	rm -f ./i8k/i8kdio_8k.o

i8kaio_8k: ./i8k/i8kaio_8k.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/i8kaio_8k.o $(LIBS)
	rm -f ./i8k/i8kaio_8k.o

demo8090: ./i8k/demo8090.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/demo8090.o $(LIBS)
	rm -f ./i8k/demo8090.o

demo8091: ./i8k/demo8091.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/demo8091.o $(LIBS)
	rm -f ./i8k/demo8091.o

setdo: ./i8k/setdo.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/setdo.o $(LIBS)
	rm -f ./i8k/setdo.o

getdi: ./i8k/getdi.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/getdi.o $(LIBS)
	rm -f ./i8k/getdi.o

setao: ./i8k/setao.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/setao.o $(LIBS)
	rm -f ./i8k/setao.o

getai: ./i8k/getai.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/getai.o $(LIBS)
	rm -f ./i8k/getai.o

sio_port: ./i8k/sio_port.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/sio_port.o $(LIBS)
	rm -f ./i8k/sio_port.o

int: ./i8k/int.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/int.o $(LIBS)
	rm -f ./i8k/int.o
	
demo8172: ./i8k/demo8172.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/demo8172.o $(LIBS)
	rm -f ./i8k/demo8172.o		
	
demo8080: ./i8k/demo8080.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/demo8080.o $(LIBS)
	rm -f ./i8k/demo8080.o	

demo8050: ./i8k/demo8050.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/demo8050.o $(LIBS)
	rm -f ./i8k/demo8050.o	

non_blocking: ./i8k/non_blocking.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/non_blocking.o $(LIBS)
	rm -f ./i8k/non_blocking.o	

blocking: ./i8k/blocking.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o ./i8k/$@ ./i8k/blocking.o $(LIBS)
	rm -f ./i8k/blocking.o	

demo1: ./gui/demo1.c
	$(CC) $(CFLAGS) $(GTKCFLAGS) $(LDFLAGS) -o ./gui/$@ ./gui/demo1.c $(LIBS)
	rm -f ./gui/demo1.o

demo2: ./gui/demo2.c
	$(CC) $(CFLAGS) $(GTKCFLAGS) $(LDFLAGS) -o ./gui/$@ ./gui/demo2.c $(LIBS)
	rm -f ./gui/demo2.o
	

#send_read: send_read.o
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ send_read.o $(LIBS)
#	rm -f send_read.o

indent:
	indent -kr -ts4 *.c

clean:
	rm -f ./common/*.o $(PJ) ./common/*~ ./common/*.tmp ./common*.bak \#* .\#*
	rm -f ./i8k/*.o $(PJ) ./i8k/*~ ./i8k/*.tmp ./i8k/*.bak \#* .\#*
	rm -f ./i87k/*.o $(PJ) ./i87k/*~ ./i87k/*.tmp ./i87k/*.bak \#* .\#*
	rm -f ./i7k/*.o $(PJ) ./i7k/*~ ./i7k/*.tmp ./i7k/*.bak \#* .\#*
	rm -f ./gui/*.o $(PJ) ./gui/*~ ./gui/*.tmp ./gui/*.bak \#* .\#*
