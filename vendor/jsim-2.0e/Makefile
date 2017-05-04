INCLUDE = include
CFLAGS = -O -g -I${INCLUDE}
LINKS = -lm
CC = cc
# for SYSV compilers that gag on ranlib (like SCO's), comment out next line
LIBRARY = ranlib

jsim: main.o jsimtxt.a device.a
	$(CC) -O -o jsim main.o jsimtxt.a device.a jsimtxt.a ${LINKS}

clean::
	rm -f *.o *.a jsimtxt/*.o device/*.o jsim

.c.o: $*.c
	$(CC) ${CFLAGS} -c $*.c


jsimtxt.a::
	cd jsimtxt; make ${LIBRARY}

device.a::
	cd device; make ${LIBRARY}


main.o:   \
  include/global.h  \
  include/jsim.h
