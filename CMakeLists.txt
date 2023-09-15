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

project( 6502 )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC version: ${CMAKE_CXX_COMPILER_VERSION}")

    add_compile_options(-std=gnu++23) 
    add_compile_options(-W -Wall -pedantic -Wuninitialized -Wshadow)
    #add_compile_options(-Weffc++)
    add_compile_options(-Werror)	#Comment this out if -Weffc++ is enabled

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC version: ${CMAKE_CXX_COMPILER_VERSION}")

    add_compile_options(/std:c++20)
    set_target_properties(${name} PROPERTIES VS_USER_PROPS do_not_import_user.props)
    #  set_target_properties(${name} PROPERTIES VS_GLOBALVcpkgEnabled false)
    #add_compile_options(/MP)	
    #add_compile_options(/wd4201 /WX)	
    find_package(fmt CONFIG REQUIRED)
    # Tnis path will have to change depending on where vcpkg is.
    #  Also, this is a hack to make this work on Windows.
    include_directories(AFTER "..\\vcpkg\\installed\\x64-windows\\include")

else()
  message(STATUS "Using unknown compiler")

endif()

add_definitions(-DBINFILE_PATH=\"${CMAKE_SOURCE_DIR}/binfiles\")
include_directories(${CMAKE_SOURCE_DIR}/src)

set(BINDIR "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(apple1)


