INCLUDE = ../include
CFLAGS = -O -g -I${INCLUDE}
CC = cc
TARGET  = ../device

COBJS =    \
cap.o      \
device.o   \
inductor.o \
isource.o  \
jj.o       \
mutual.o   \
resistor.o \
vsource.o  \
xline.o

.c.o: $*.c ${INCLUDE}/jsim.h ${INCLUDE}/extern.h
	$(CC) ${CFLAGS} -c $*.c

 
library:  ${TARGET}.a
${TARGET}.a:  ${COBJS}
	ar r ${TARGET}.a $?

ranlib:  ${TARGET}.a
	ranlib ${TARGET}.a
