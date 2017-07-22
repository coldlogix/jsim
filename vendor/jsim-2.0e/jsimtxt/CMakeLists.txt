cmake_minimum_required(VERSION 2.8)

project(jsimtxt C)

set (JSTSRC 
	debug.c 
	function.c 
	matrix.c 
	misc.c 
	model.c 
	morspace.c 
	picture.c 
	print.c 
	setup.c 
	srchsort.c 
	subckt.c 
	topology.c 
	tran.c 
)

add_library(jsimtxt
    ${JSTSRC} 
)

target_include_directories(jsimtxt
  PUBLIC
    ../include
)

