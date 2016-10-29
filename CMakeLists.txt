cmake_minimum_required (VERSION 2.8.11)
project (dbccpp)

set( CMAKE_VERBOSE_MAKEFILE on )

file(GLOB SOURCES_DBCCPP include/dbccpp/*.h src/*.cpp src/*.h src/mysql/*.cpp src/mysql/*.h src/sqlite/*.cpp src/sqlite/*.h )
file(GLOB SOURCES_TEST test/src/*.cpp test/testcpp/src/*.cpp  )

include_directories(include test/testcpp/include  )

link_directories(lib)

add_library (libdbccpp STATIC ${SOURCES_DBCCPP})

add_executable(dbccpp-test ${SOURCES_TEST} )

target_link_libraries(dbccpp-test libdbccpp.a)

