include(CompileOptions)
include(Conan)

find_package(tl-expected REQUIRED)

add_library(peripp-i2c i2c.cpp)
add_library(peripp::i2c ALIAS peripp-i2c)
target_link_libraries(peripp-i2c PUBLIC tl::expected peripp::headers)
target_compile_features(peripp-i2c PRIVATE cxx_std_20)
target_compile_options(peripp-i2c PRIVATE ${warnings} ${wae})
