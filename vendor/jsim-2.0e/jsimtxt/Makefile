INCLUDE = ../include
CFLAGS = -O -g -I${INCLUDE}
CC = cc
TARGET  = ../jsimtxt

COBJS =     \
 debug.o    \
 function.o \
 matrix.o   \
 misc.o     \
 model.o    \
 morspace.o \
 picture.o  \
 print.o    \
 setup.o    \
 srchsort.o \
 subckt.o   \
 topology.o \
 tran.o

.c.o: $*.c ${INCLUDE}/jsim.h ${INCLUDE}/extern.h
	$(CC) ${CFLAGS} -c $*.c

 
library:  ${TARGET}.a
${TARGET}.a:  ${COBJS}
	ar r ${TARGET}.a $?

ranlib:  ${TARGET}.a
	ranlib ${TARGET}.a
