cmake_minimum_required(VERSION 2.8.12)
project(shasum)

#
# Configures the compiler, regardless of the build type.
#
if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
  add_definitions(-std=c++11                      # Use C++ 2011, not 2008.
                  -stdlib=libc++                  # Use the modern C++ library
                  -Weverything                    # Turn on everything.
                  -Wno-c++98-compat               # Don't care about C98.
                  -Wno-missing-prototypes         # Not necessary.
                  -Wno-deprecated-declarations    # OpenSSL is deprecated on Mac.
                  )
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
  add_definitions(-std=c++11                      # Use C++ 2011, not 2008.
                  -Wall                           # Turn on everything.
                  -Wextra                         # Turn on everything.
                  -Wpedantic                      # Turn on everything.
                  )
endif()

#
# Use `make doc` to generate API documentation.
#
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html
          DESTINATION doc)
endif(DOXYGEN_FOUND)

#
# Build dependencies
#
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

#
# Builds the executable.
#
set(
  SOURCE
  source/main.cpp
  )
add_executable(shasum ${SOURCE})
target_link_libraries(shasum ${OPENSSL_LIBRARIES})
