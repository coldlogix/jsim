cmake_minimum_required(VERSION 2.8)

project(jsim C)

add_subdirectory(device)
add_subdirectory(jsimtxt)

add_executable(jsim
    main.c

    include/jsim.h
    include/global.h
    include/extern.h
    include/setup.h
)

target_include_directories(jsim
  PUBLIC
    include
)

target_link_libraries(jsim
  PUBLIC
    jsimdevice
    jsimtxt
)


add_executable(jsim_n
    main_n.c

    include/jsim_n.h
    include/global.h
    include/extern_n.h
    include/setup.h
)

target_include_directories(jsim_n
  PUBLIC
    include
)

target_link_libraries(jsim_n
  PUBLIC
    jsim_ndevice
    jsim_ntxt
)
#doc: 
#	nroff -ms manual.ms
#	nroff -ms noise.ms

