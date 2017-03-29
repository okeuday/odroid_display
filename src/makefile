#-*-Mode:make;coding:utf-8;tab-width:4;c-basic-offset:4-*-
# ex: set ft=make fenc=utf-8 sts=4 ts=4 sw=4 noet:
CC=gcc
CLOUDI_VERSION=1.7.0
CLOUDI_PATH=/usr/local/lib/cloudi-$(CLOUDI_VERSION)
CLOUDI_CFLAGS=-I$(CLOUDI_PATH)/api/c/ -fexceptions
CLOUDI_LDFLAGS=-L$(CLOUDI_PATH)/api/c/ -Wl,-rpath,$(CLOUDI_PATH)/api/c/ -lcloudi
CFLAGS=-g -O0 $(CLOUDI_CFLAGS)
LDFLAGS=-lwiringPi -lwiringPiDev -lpthread $(CLOUDI_LDFLAGS)

OBJS = \
    utf8_to_hd44780u.o \
    main.o

all: odroid_display

utf8_to_hd44780u.c: utf8_to_hd44780u.h
main.c: utf8_to_hd44780u.h

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	rm -f odroid_display $(OBJS)

odroid_display: $(OBJS)
	$(CC) -o odroid_display $(OBJS) $(LDFLAGS)

