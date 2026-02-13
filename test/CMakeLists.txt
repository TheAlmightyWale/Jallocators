include(GoogleTest)

set(TARGET_NAME "Tests")

add_executable(${TARGET_NAME}   "IntrusiveStackTests.cpp")
set_target_properties(${TARGET_NAME}  PROPERTIES CXX_STANDARD 20)
target_include_directories(${TARGET_NAME}  PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(${TARGET_NAME} PUBLIC gtest_main PUBLIC ${LIBRARY_NAME})

gtest_discover_tests(${TARGET_NAME})
