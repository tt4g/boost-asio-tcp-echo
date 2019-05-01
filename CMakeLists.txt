cmake_minimum_required(VERSION 3.4.0)

project(BoostAsioTcpEcho VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

################################################################################
# cmake-conan
# https://github.com/conan-io/cmake-conan

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.13/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include("${CMAKE_BINARY_DIR}/conan.cmake")
conan_cmake_run(
  CONANFILE conanfile.txt
  BASIC_SETUP CMAKE_TARGETS
)

# cmake-conan
# https://github.com/conan-io/cmake-conan
################################################################################

add_executable(BoostAsioTcpEcho
  src/main.cpp
  src/client/ClientLayer.cpp
  src/client/ConnectCommand.cpp
  src/client/ReadCommand.cpp
  src/client/ResolveCommand.cpp
  src/client/Settings.cpp
  src/client/WriteCommand.cpp
)

target_include_directories(BoostAsioTcpEcho
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#    $<INSTALL_INTERFACE:include/mylib>
)

target_link_libraries(BoostAsioTcpEcho
  PUBLIC
    CONAN_PKG::boost
)

target_compile_options(BoostAsioTcpEcho
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<VERSION_GREATER_EQUAL:${MSVC_VERSION},1900>>:/permissive->
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)

# Boost.Asio
# Disable autolink: https://www.boost.org/doc/libs/1_70_0/doc/html/boost_asio/using.html#boost_asio.using.dependencies
target_compile_definitions(BoostAsioTcpEcho
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB>
)

# Fixes MSVC 2017 warning bug: https://github.com/boostorg/asio/issues/109
target_compile_definitions(BoostAsioTcpEcho
  PUBLIC
    $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<VERSION_GREATER_EQUAL:${MSVC_VERSION},1900>>:_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING>
)
