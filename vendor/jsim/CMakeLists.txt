cmake_minimum_required(VERSION 2.8)

project(jsim C)

add_subdirectory(device)
add_subdirectory(jsimtxt)
add_subdirectory(jsim_n-tests)

include(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(pow RESULT)
if(NOT RESULT)
  unset(RESULT)
  list(APPEND CMAKE_REQUIRED_LIBRARIES m)
  CHECK_FUNCTION_EXISTS(pow RESULT)
  if(RESULT)
    list(APPEND LIBS m)
  else()
    message(FATAL_ERROR "No pow() found")
  endif()
endif()

if (WIN32)
    # Win32 needs to know to define M_PI
    add_definitions(-D_USE_MATH_DEFINES)
endif (WIN32)

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
    jsimtxt
    jsimdevice
    ${LIBS}
)


add_executable(jsim_n
    main_n.c

    include/jsim.h
    include/global.h
    include/setup.h
)

target_include_directories(jsim_n
  PUBLIC
    include
)

target_link_libraries(jsim_n
  PUBLIC
    jsimtxt
    jsimdevice
    ${LIBS}
)
#doc: 
#	nroff -ms manual.ms
#	nroff -ms noise.ms

