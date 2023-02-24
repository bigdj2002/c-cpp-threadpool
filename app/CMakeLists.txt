file(GLOB_RECURSE MAIN_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)
FetchContent_MakeAvailable(googletest)

add_executable(program ${MAIN_FILES})
target_link_libraries(program PUBLIC add)
target_include_directories(program 
  PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/app/include
    ${CMAKE_SOURCE_DIR}/libs/add/include
)

add_executable(unitTest ${TEST_FILES})
target_link_libraries(unitTest PUBLIC add gtest gmock gtest_main)
target_include_directories(unitTest 
  PUBLIC 
    ${CMAKE_SOURCE_DIR}/libs/add/include
)