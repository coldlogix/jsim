cmake_minimum_required(VERSION 2.8)

project(jsimdevice C)

set (DEVSRC
	cap.c 
	device.c 
	inductor.c 
	isource.c 
	jj.c 
	mutual.c 
	resistor.c 
	vsource.c 
	xline.c
)

add_library(jsimdevice
    ${DEVSRC}
)

target_include_directories(jsimdevice
  PUBLIC
    ../include
)
