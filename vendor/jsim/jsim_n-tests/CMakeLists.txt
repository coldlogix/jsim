cmake_minimum_required(VERSION 2.8)

project(jsim_n_test C)


include(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(pow NOT_NEED_LIBM)
if(NOT_NEED_LIBM)
  set(LIB_MATH "")
else()
  set(CMAKE_REQUIRED_LIBRARIES m)
  CHECK_FUNCTION_EXISTS(pow HAVE_LIBM)
  if(HAVE_LIBM)
    set(LIB_MATH m)
  else()
    message(FATAL_ERROR "No libm found for math support")
  endif()
endif()

if (WIN32)
    # Win32 needs to know to define M_PI
    add_definitions(-D_USE_MATH_DEFINES)
endif (WIN32)

add_executable(ah
    ah.c
)

target_link_libraries(ah
  PUBLIC
    ${LIB_MATH}
)

add_executable(rt
    rt.c
)

target_link_libraries(rt
  PUBLIC
    ${LIB_MATH}
)
