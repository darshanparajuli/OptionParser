cmake_minimum_required(VERSION 3.7)

set(HEADER_FILES includes/option_parser.h)
set(SOURCE_FILES option_parser.cpp ${HEADER_FILES})
add_library(OptionParser ${SOURCE_FILES})

if (${CMAKE_BUILD_TYPE} MATCHES Release)
    install(TARGETS OptionParser DESTINATION lib)
    install(FILES ${HEADER_FILES} DESTINATION include/option_parser)
endif ()
