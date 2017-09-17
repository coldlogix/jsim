cmake_minimum_required(VERSION 2.8)

project(jsimtxt C)

set (JSTCOMMONSRC 
    debug.c
    function.c
    matrix.c
    misc.c
    model.c
    morspace.c 
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
)

target_include_directories(jsimtxt
  PUBLIC
    ../include
)



