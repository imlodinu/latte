set(LATTE_TEST_SOURCES main.cpp)

add_executable(latte_test ${LATTE_TEST_SOURCES})
target_link_libraries(latte_test PUBLIC latte)