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

project( 6502_tests )

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
# 6502 tests
# source for the test executable
set (6502_TEST_SOURCES
  "6502_tests_adc.cc"
  "6502_tests_and.cc"
  "6502_tests_asl.cc"
  "6502_tests_bit.cc"
  "6502_tests_branches.cc"
  "6502_tests_brk.cc"
  "6502_tests_cmp.cc"
  "6502_tests_dec_dex_dey.cc"
  "6502_tests_eor.cc"
  "6502_tests_flags.cc"
  "6502_tests_inc_inx_iny.cc"
  "6502_tests_interrupt.cc"
  "6502_tests_invalid_instruction.cc"
  "6502_tests_jmp.cc"
  "6502_tests_jsr.cc"
  "6502_tests_lda_ldy_ldx.cc"
  "6502_tests_load_program.cc"
  "6502_tests_lsr.cc"
  "6502_tests_nop.cc"
  "6502_tests_ora.cc"
  "6502_tests_pushpop.cc"
  "6502_tests_rol_ror.cc"
  "6502_tests_rti.cc"
  "6502_tests_rts.cc"
  "6502_tests_tx_ty.cc"
  "6502_tests_xxx_functional_test_suite.cc"
)

source_group("src" FILES ${6502_TEST_SOURCES})
add_executable( 6502_tests ${6502_TEST_SOURCES})
add_dependencies(6502_tests 6502-test)
add_dependencies(6502_tests gtest_main)
target_link_libraries(6502_tests gtest_main 6502-test)
if(MSVC)
  target_link_libraries(6502_tests fmt::fmt-header-only)
else()
  target_compile_definitions(6502_tests PRIVATE FMT_HEADER_ONLY)  
	target_link_libraries(6502_tests readline)
endif()
target_compile_definitions(6502_tests PRIVATE TEST_BUILD)
add_custom_target(run_6502_tests
  DEPENDS 6502_tests
  COMMAND 6502_tests)

# Phony targets 
add_custom_target(tests
  DEPENDS memory_tests 6502_tests)
add_custom_target(runtests
  DEPENDS run_memory_tests run_6502_tests)

# Copy the test binarys to {builddir}/bin
add_custom_command(TARGET memory_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:memory_tests>" "${BINDIR}")
add_custom_command(TARGET 6502_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:6502_tests>" "${BINDIR}")
