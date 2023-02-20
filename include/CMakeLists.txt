add_library(peripp-headers INTERFACE)
add_library(peripp::headers ALIAS peripp-headers)
target_include_directories(peripp-headers INTERFACE "${CMAKE_CURRENT_DIRECTORY}")
target_compile_features(peripp-headers INTERFACE cxx_std_20)
