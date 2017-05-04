cmake_minimum_required(VERSION 2.6)

project(jsim_n C)

INCLUDE_DIRECTORIES(
	include
)

set (DEVSRC
	device/cap.c device/device_n.c device/inductor.c device/isource.c 
	device/jj.c device/mutual.c device/resistor.c device/vsource.c device/xline.c
)

set (JSTSRC 
	jsimtxt/debug.c jsimtxt/function_n.c jsimtxt/matrix.c 
	jsimtxt/misc_n.c jsimtxt/model.c jsimtxt/morspace.c jsimtxt/picture.c 
	jsimtxt/print.c jsimtxt/setup.c jsimtxt/srchsort.c jsimtxt/subckt_n.c 
	jsimtxt/topology_n.c jsimtxt/tran_n.c 
)

add_executable(jsim_n
    ${JSTSRC} ${DEVSRC} main_n.c
)

#doc: 
#	nroff -ms manual.ms
#	nroff -ms noise.ms

