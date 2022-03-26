add_executable(line_count line_count.cpp)
add_executable(calculate_average calculate_average.cpp)
target_link_libraries(calculate_average TBB::tbb)