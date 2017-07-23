cmake_minimum_required(VERSION 2.8)

project(jsimtxt C)

set (JSTCOMMONSRC 
    debug.c 
    matrix.c
    misc.c
    model.c
    morspace.c 
    picture.c
    print.c
    setup.c
    subckt.c
    srchsort.c
    topology.c
    tran.c
    ../include/setup.h
    ../include/jsim.h
    ../include/extern.h
)

add_library(jsimtxt
    ${JSTCOMMONSRC} 
    function.c 
)

add_library(jsim_ntxt
    ${JSTCOMMONSRC} 
    function_n.c
)

target_include_directories(jsimtxt
  PUBLIC
    ../include
)

target_include_directories(jsim_ntxt
  PUBLIC
    ../include
)



