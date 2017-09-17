cmake_minimum_required(VERSION 2.8)

project(jsim_n_test C)


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

add_executable(ah
    ah.c
)

target_link_libraries(ah
  PUBLIC
    ${LIBS}
)

add_executable(rt
    rt.c
)

target_link_libraries(rt
  PUBLIC
    ${LIBS}
)
