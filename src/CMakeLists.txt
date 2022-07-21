add_executable(line_count line_count.cpp)
if (UNIX)
    add_executable(calculate_average calculate_average.cpp)
    target_link_libraries(calculate_average TBB::tbb)
endif()
add_executable(lambda lambda.cpp)
add_executable(fib fib.cpp)
add_executable(concat concat.cpp)
add_executable(ranges ranges.cpp)
target_link_libraries(ranges PRIVATE
        range-v3::range-v3
        )
add_executable(action action.cpp)
target_link_libraries(action range-v3::range-v3)
add_executable(word_frequency word_frequency.cpp)
target_link_libraries(word_frequency PRIVATE
        range-v3::range-v3
        )

add_executable(tie tie.cpp)