# Copyright (C) 2023 Walt Drummond
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0...3.7)

project( tests )

## Google Test configuration
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
  ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest-src/googletest/include/gtest)
##

##########
## 6502 and 65C02 processor tests
add_subdirectory(6502)
add_subdirectory(65C02)

##########
# Memory tests
set (MEMORY_TEST_SOURCES
  "memory_tests.cc"
)

source_group("src" FILE ${MEMORY_TEST_SOURCES})
add_executable(memory_tests ${MEMORY_TEST_SOURCES})
add_dependencies(memory_tests gtest_main)
target_link_libraries(memory_tests gtest_main)
if(MSVC)
  target_link_libraries(memory_tests fmt::fmt-header-only)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS "Adding ${LIBCXXABI_LIBRARY} to link_libraries for memory_test")
  target_link_libraries(memory_tests ${LIBCXXABI_LIBRARY})
else()
  target_compile_definitions(memory_tests PRIVATE FMT_HEADER_ONLY)
  target_link_libraries(memory_tests readline)
endif()
add_custom_target(run_memory_tests
  DEPENDS memory_tests
  COMMAND memory_tests)

##########
# Clock tests
set (CLOCK_TEST_SOURCES
"clock_tests.cc"
)

source_group("src" FILE ${CLOCK_TEST_SOURCES})
add_executable(clock_tests ${CLOCK_TEST_SOURCES})
add_dependencies(clock_tests gtest_main)
target_link_libraries(clock_tests gtest_main)
if(MSVC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
message(STATUS "Adding ${LIBCXXABI_LIBRARY} to link_libraries for clock_test")
target_link_libraries(clock_tests ${LIBCXXABI_LIBRARY})
else()
endif()
add_custom_target(run_clock_tests
  DEPENDS clock_tests
  COMMAND clock_tests)

# Phony targets 
add_custom_target(tests
  DEPENDS memory_tests clock_tests 6502_tests 65C02_tests)
add_custom_target(runtests
  DEPENDS run_memory_tests run_clock_tests run_6502_tests)

# Copy the test binarys to {builddir}/bin
add_custom_command(TARGET memory_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:memory_tests>" "${BINDIR}")
add_custom_command(TARGET clock_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:clock_tests>" "${BINDIR}")

