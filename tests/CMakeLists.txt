include(CTest)
if(NOT BUILD_TESTING)
  return()
endif()

include(CompileOptions)
include(Conan)

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

macro(construct_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain fmt::fmt)
    add_test(NAME ${name} COMMAND ${name})
endmacro()

construct_test(test-i2c i2c.cpp)
target_compile_options(test-i2c PRIVATE ${warnings})
target_link_libraries(test-i2c PRIVATE peripp::i2c)
