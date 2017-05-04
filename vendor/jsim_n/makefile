#
#********************************************************************
# jsim_n 3/8/95	                                                     *
# stochastic extension of jsim.                                      *
#  (c)British Crown copyright January 1997/DERA.                                 #
#  Permission to use, copy, modify, and distribute this software     * 
#  for any purpose without fee is hereby granted, provided that the  *
#  above copyright notice appears in all copies. The copyright       *
#  holders make no representations about the suitability of this     *
#  software for any purpose. It is provided "as is" without express  *
#  or implied warranty. No liability is accepted by the copyright    *
#  holder for any use made of this software                          *
#*********************************************************************/
#
SHELL = /bin/csh
INCLUDE = include
# Change this if you nwant some other compiler
CC = gcc

# Slow compile, fast run, production version
CFLAGS = -O3 -I$(INCLUDE) 
# CFLAGS = -O0 -g 
DEVOBJ = device/cap.o device/device_n.o device/inductor.o device/isource.o \
	device/jj.o device/mutual.o device/resistor.o device/vsource.o device/xline.o

DEVSRC = device/cap.c device/device_n.c device/inductor.c device/isource.c \
	device/jj.c device/mutual.c device/resistor.c device/vsource.c device/xline.c

JSTOBJ = jsimtxt/debug.o jsimtxt/function_n.o jsimtxt/matrix.o \
       jsimtxt/misc_n.o jsimtxt/model.o jsimtxt/morspace.o jsimtxt/picture.o \
       jsimtxt/print.o jsimtxt/setup.o jsimtxt/srchsort.o jsimtxt/subckt_n.o \
       jsimtxt/topology_n.o jsimtxt/tran_n.o 

JSTSRC = jsimtxt/debug.c jsimtxt/function_n.c jsimtxt/matrix.c \
       jsimtxt/misc_n.c jsimtxt/model.c jsimtxt/morspace.c jsimtxt/picture.c \
       jsimtxt/print.c jsimtxt/setup.c jsimtxt/srchsort.c jsimtxt/subckt_n.c \
       jsimtxt/topology_n.c jsimtxt/tran_n.c 

LINKS = -lm

jsim_n: main_n.o $(JSTOBJ) $(DEVOBJ)
	$(CC) -O -o jsim_n main_n.o $(JSTOBJ) $(DEVOBJ) $(LINKS)

clean:
	rm -f *.o *.a jsimtxt/*.o device/*.o

.c.o: $*.c
	$(CC) $(CFLAGS) -c -o $@  $<


doc: 
	nroff -ms manual.ms
	nroff -ms noise.ms
	
main_n.o:  include/global.h include/jsim_n.h
