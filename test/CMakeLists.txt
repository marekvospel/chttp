add_executable(example_test example.spec.c)

target_link_libraries(example_test ${CRITERION_LIBRARIES})

add_test (example_test example_test)
