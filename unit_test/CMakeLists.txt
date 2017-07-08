cmake_minimum_required(VERSION 3.7)

set(GTEST_LIBRARY /usr/lib/libgtest.so /usr/lib/libgtest_main.so)

add_executable(OptionParserUnitTest option_parser_test.cpp ${SOURCE_FILES})
target_link_libraries(OptionParserUnitTest OptionParser)
target_link_libraries(OptionParserUnitTest ${GTEST_LIBRARY})