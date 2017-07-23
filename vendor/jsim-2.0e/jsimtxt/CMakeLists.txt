cmake_minimum_required(VERSION 2.8)

project(jsimtxt C)

set (JSTCOMMONSRC 
    debug.c 
    matrix.c 
    model.c 
    morspace.c 
    picture.c 
    setup.c 
    srchsort.c 
    ../include/setup.h
    ../include/jsim.h
)

add_library(jsimtxt
    ${JSTCOMMONSRC} 
    function.c 
    misc.c 
    print.c 
    subckt.c 
    topology.c 
    tran.c 
    ../include/extern.h
)

add_library(jsim_ntxt
    ${JSTCOMMONSRC} 
    function_n.c
    misc_n.c
    print_n.c 
    subckt_n.c
    topology_n.c
    tran_n.c
    ../include/extern_n.h 
)

target_include_directories(jsimtxt
  PUBLIC
    ../include
)

target_include_directories(jsim_ntxt
  PUBLIC
    ../include
)



