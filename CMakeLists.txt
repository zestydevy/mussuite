#MusSuite CMakeLists

cmake_minimum_required(VERSION 3.1)

project(MusSuite VERSION 0.1)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRCS
    include/*.hpp
    src/*.cpp
)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)

FetchContent_Declare(midifile
  GIT_REPOSITORY https://github.com/craigsapp/midifile.git
  GIT_TAG master
)

FetchContent_MakeAvailable(fmt midifile)

include_directories(${midifile_SOURCE_DIR}/include)

add_executable(MusSuite ${SRCS})

target_link_libraries(MusSuite 
  PRIVATE 
    fmt::fmt
  PRIVATE
    midifile
)