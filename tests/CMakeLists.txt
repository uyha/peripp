include(CompileOptions)
include(Conan)

find_package(Catch2 REQUIRED)
find_package(fmt REQUIRED)

macro(construct_test name)
  cmake_parse_arguments(arg ";" ";" "SOURCES;TEST_ARGUMENTS" ${ARGN})
  add_executable(${name} ${arg_SOURCES})
  target_link_libraries(${name} PRIVATE Catch2::Catch2WithMain fmt::fmt)
  target_compile_options(${name} PRIVATE ${warnings} ${wae})
  add_test(NAME ${name} COMMAND ${name} ${arg_TEST_ARGUMENTS})
endmacro()

construct_test(test-i2c-unreal SOURCES i2c.cpp)
target_link_libraries(test-i2c-unreal PRIVATE peripp::i2c)

construct_test(test-i2c-virtual SOURCES i2c.cpp TEST_ARGUMENTS [.virtual],*)
target_link_libraries(test-i2c-virtual PRIVATE peripp::i2c)
