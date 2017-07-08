cmake_minimum_required(VERSION 3.7)

add_executable(OptionParserTest main.cpp ${SOURCE_FILES})
target_link_libraries(OptionParserTest OptionParser)