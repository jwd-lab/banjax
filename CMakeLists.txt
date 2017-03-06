cmake_minimum_required (VERSION 2.8)
################################################################################
set(CMAKE_BUILD_TYPE Debug)
set(GLOB BOOST_VERSION 1.58)

################################################################################
project (banjax)

find_package(Boost ${BOOST_VERSION} COMPONENTS REQUIRED)
find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -Wall -ggdb")

include_directories(
  "${Boost_INCLUDE_DIR}"
  "${CMAKE_SOURCE_DIR}/include")

file(GLOB sources
  "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(banjax_static STATIC ${sources})
set_target_properties(banjax_static PROPERTIES OUTPUT_NAME banjax)
target_link_libraries(banjax_static ssl crypto re2 zmq yaml-cpp)

add_library(banjax_module MODULE ${sources})
set_target_properties(banjax_module PROPERTIES OUTPUT_NAME banjax)
target_link_libraries(banjax_module ssl crypto re2 zmq yaml-cpp)
set_target_properties(banjax_module PROPERTIES PREFIX "")


################################################################################
project (unit-tests)

find_package(Boost ${BOOST_VERSION} COMPONENTS system unit_test_framework REQUIRED)
find_package(Threads)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 \
                                        -pthread \
                                        -ggdb")

include_directories(
  "${Boost_INCLUDE_DIR}"
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/test")

file(GLOB sources
  "${CMAKE_SOURCE_DIR}/test/unittest_main.cpp"
  "${CMAKE_SOURCE_DIR}/test/challenge_manager_unittest.cpp"
  "${CMAKE_SOURCE_DIR}/test/regex_manager_unittest.cpp"
  "${CMAKE_SOURCE_DIR}/test/unittest_common.cpp"
  "${CMAKE_SOURCE_DIR}/test/white_lister_unittest.cpp"
)

add_executable(unit-tests ${sources})
target_link_libraries(unit-tests banjax_static ${Boost_LIBRARIES})

################################################################################