add_executable(parse_test parse_test.c)

target_link_libraries(parse_test ${CRITERION_LIBRARIES})

add_test(parse_test parse_test)
