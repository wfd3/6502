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

project( 65C02_tests )

##########
# 65C02 tests
# source for the test executable

# 6502 instructions
set (6502_TEST_SOURCES
"65C02_tests_adc.cc"
"65C02_tests_and.cc"
"65C02_tests_asl.cc"
"65C02_tests_bit.cc"
"65C02_tests_branches.cc"
"65C02_tests_brk.cc"
"65C02_tests_cmp.cc"
"65C02_tests_dec_dex_dey.cc"
"65C02_tests_eor.cc"
"65C02_tests_flags.cc"
"65C02_tests_inc_inx_iny.cc"
"65C02_tests_interrupt.cc"
"65C02_tests_invalid_instruction.cc"
"65C02_tests_jmp.cc"
"65C02_tests_jsr.cc"
"65C02_tests_lda_ldy_ldx.cc"
"65C02_tests_load_program.cc"
"65C02_tests_lsr.cc"
"65C02_tests_nop.cc"
"65C02_tests_ora.cc"
"65C02_tests_pushpop.cc"
"65C02_tests_rol_ror.cc"
"65C02_tests_rti.cc"
"65C02_tests_rts.cc"
"65C02_tests_tx_ty.cc"
)

# 65C02 instructions
set (65C02_TEST_SOURCES
"65C02_tests_stz.cc"
"65C02_tests_phy_ply_phx_plx.cc"
"65C02_tests_trb.cc"
"65C02_tests_tsb.cc"
)

# Additional Rockwell 65C02 instructions
set (R65C02_TEST_SOURCES 
"R65C02_tests_rmb.cc"
"R65C02_tests_bbr_bbs.cc"
)

# 65C02 functional tests
set (65C02_FUNCTIONAL_TEST_SOURCES
"65C02_tests_functional_test_suite.cc"
)

# Regression tests
source_group("src" FILES ${65C02_TEST_SOURCES} ${R65C02_TEST_SOURCES} ${6502_TEST_SOURCES})
add_executable( 65C02_tests ${65C02_TEST_SOURCES} ${R65C02_TEST_SOURCES} ${6502_TEST_SOURCES})

add_dependencies(65C02_tests 65C02-test 6502-test)
add_dependencies(65C02_tests gtest_main)
target_link_libraries(65C02_tests gtest_main 65C02-test 6502-test)
if(MSVC)
target_link_libraries(65C02_tests fmt::fmt-header-only)
else()
target_compile_definitions(65C02_tests PRIVATE FMT_HEADER_ONLY)  
target_link_libraries(65C02_tests readline)
endif()
target_compile_definitions(65C02_tests PRIVATE TEST_BUILD)
add_custom_target(run_65C02_tests
DEPENDS 65C02_tests
COMMAND 65C02_tests)

# Functional tests
source_group("src" FILES ${65C02_FUNCTIONAL_TEST_SOURCES})
add_executable( 65C02_functional_tests ${65C02_FUNCTIONAL_TEST_SOURCES})

add_dependencies(65C02_functional_tests 65C02-test 6502-test)
add_dependencies(65C02_functional_tests gtest_main)
target_link_libraries(65C02_functional_tests gtest_main 65C02-test 6502-test)
if(MSVC)
 target_link_libraries(65C02_functional_tests fmt::fmt-header-only)
else()
 target_compile_definitions(65C02_functional_tests PRIVATE FMT_HEADER_ONLY)  
 target_link_libraries(65C02_functional_tests readline)
endif()
target_compile_definitions(65C02_functional_tests PRIVATE TEST_BUILD)
add_custom_target(run_65C02_functional_tests 
DEPENDS 65C02_functional_tests
COMMAND 65C02_functional_tests)

add_custom_command(TARGET 65C02_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:65C02_tests>" "${BINDIR}")
add_custom_command(TARGET 65C02_functional_tests POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:65C02_functional_tests>" "${BINDIR}")
