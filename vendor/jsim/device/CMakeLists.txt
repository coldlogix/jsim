cmake_minimum_required(VERSION 2.8)

project(jsimdevice C)

set (COMMONDEVSRC
    cap.c
    device.c
    inductor.c
    isource.c
    jj.c
    mutual.c 
    resistor.c 
    vsource.c 
    xline.c
    picture.c
    ../include/extern.h
)

add_library(jsimdevice
    ${COMMONDEVSRC}
    ../include/jsim.h
)

target_include_directories(jsimdevice
  PUBLIC
    ../include
)
