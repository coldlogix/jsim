cmake_minimum_required(VERSION 2.8)

project(jsimdevice C)

set (COMMONDEVSRC
	cap.c 
	inductor.c 
	isource.c 
	jj.c 
	mutual.c 
	resistor.c 
	vsource.c 
	xline.c
)

add_library(jsimdevice
    ${COMMONDEVSRC}
    device.c 
    ../include/extern.h
    ../include/jsim.h
)

add_library(jsim_ndevice
    ${COMMONDEVSRC}
    device_n.c 
    ../include/extern_n.h
    ../include/jsim.h
)

target_include_directories(jsimdevice
  PUBLIC
    ../include
)

target_include_directories(jsim_ndevice
  PUBLIC
    ../include
)
