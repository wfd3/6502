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

cmake_minimum_required(VERSION 3.7)

project( lib65C02 )

set (SOURCES "65C02.h" "65C02.cc" )
source_group("src" FILES ${SOURCES})
include_directories(AFTER ${fmt_INCLUDE_DIRS})

add_library( 65C02 ${SOURCES} )
target_link_libraries(65C02 PRIVATE 6502)
target_compile_definitions(65C02 PRIVATE FMT_HEADER_ONLY)

add_library( 65C02-test ${SOURCES})
target_link_libraries(65C02-test PRIVATE 6502)
target_compile_definitions(65C02-test PRIVATE FMT_HEADER_ONLY TEST_BUILD)