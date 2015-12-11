add_executable(TestSegmentation TestSegmentation.cpp)
add_executable(TestGraphLibrary TestGraphLibrary.cpp)

target_link_libraries(TestSegmentation gtest gtest_main ${ITK_LIBRARIES} KolmogorovMaxFlow)
target_link_libraries(TestGraphLibrary gtest gtest_main ${ITK_LIBRARIES} ${Boost_LIBRARIES} KolmogorovMaxFlow)