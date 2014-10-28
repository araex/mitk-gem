add_executable(runTests Test.cpp)
target_link_libraries(runTests gtest gtest_main ${ITK_LIBRARIES} KolmogorovMaxFlow)