add_library(add 
  STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/add.cpp
)

target_include_directories(add PRIVATE include)
target_link_libraries(add PRIVATE -lpthread)
target_compile_options(add PRIVATE -Wall -Werror)