#Cmake file for examples of how to use my command line parsing code.

#The basics
cmake_minimum_required(VERSION 2.6)
project(CMD)
#set(CMAKE_VERBOSE_MAKEFILE ON)


#Find files
set(SRCS_EX1 src/example_simple.cpp    src/CommandLineParser.cpp src/IncludedArgParsers.cpp)

#Executables
set(EX1_APP  bin/ex_simple    )

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

#The executables
add_executable(${EX1_APP} ${SRCS_EX1})

#------------------------------------------------------------------------------
#Below this line is for making the Doxygen documentation.  Comment everything below here
#out if you don't care about this.
#This code snippet was taken from: http://majewsky.wordpress.com/2010/08/14/tip-of-the-day-cmake-and-doxygen/ 

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
