##------------------------------------------------------------------------------
## avigle-io -- common io classes/tools
##
## Developed during the research project AVIGLE
## which was part of the Hightech.NRW research program
## funded by the ministry for Innovation, Science, Research and Technology
## of the German state Northrhine-Westfalia, and by the European Union.
##
## Copyright (c) 2010--2013, Tom Vierjahn et al.
##------------------------------------------------------------------------------
##                                License
##
## This library/program is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published
## by the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## If you are using this library/program in a project, work or publication,
## please cite [1,2].
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##------------------------------------------------------------------------------
##                                References
##
## [1] S. Rohde, N. Goddemeier, C. Wietfeld, F. Steinicke, K. Hinrichs,
##     T. Ostermann, J. Holsten, D. Moormann:
##     "AVIGLE: A System of Systems Concept for an
##      Avionic Digital Service Platform based on
##      Micro Unmanned Aerial Vehicles".
##     In Proc. IEEE Int'l Conf. Systems Man and Cybernetics (SMC),
##     pp. 459--466. 2010. DOI: 10.1109/ICSMC.2010.5641767
## [2] S. Strothoff, D. Feldmann, F. Steinicke, T. Vierjahn, S. Mostafawy:
##     "Interactive generation of virtual environments using MUAVs".
##     In Proc. IEEE Int. Symp. VR Innovations, pp. 89--96, 2011.
##     DOI: 10.1109/ISVRI.2011.5759608
##------------------------------------------------------------------------------

################################################################################
### multiple include guard
################################################################################
if (NOT TARGET io)



################################################################################
### cmake
################################################################################
cmake_minimum_required(VERSION 2.8)
#if (NOT CMAKE_MODULE_PATH)
#  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)
#endif()



################################################################################
### project info / version
################################################################################
project(io)
set(VERSION_MAJOR 16)	# year
set(VERSION_MINOR 12)	# month
set(VERSION_PATCH 15)	# day
set(VERSION_TWEAK 00)	# count


# produce version string
set(VERSION_STR "${VERSION_MAJOR}")
set(VERSION_STR "${VERSION_STR}.${VERSION_MINOR}")
set(VERSION_STR "${VERSION_STR}.${VERSION_PATCH}")
set(VERSION_STR "${VERSION_STR}.${VERSION_TWEAK}")

# produce version int
string(REPLACE "." "" VERSION_INT ${VERSION_STR})

# compile version into headers
configure_file(
  ${PROJECT_SOURCE_DIR}/include/io/version.in
  ${PROJECT_SOURCE_DIR}/include/io/version.h
  @ONLY)

# use version as postfix
set(CMAKE_DEBUG_POSTFIX "-${VERSION_STR}d")
set(CMAKE_RELEASE_POSTFIX "-${VERSION_STR}")


################################################################################
### source files
################################################################################
file(GLOB IO_SOURCES src/*)
file(GLOB IO_HEADERS include/io/*)

### include directories ###
include_directories(./include/)



################################################################################
### dependencies
################################################################################

### boost ###
find_package(Boost REQUIRED filesystem system)
if(MSVC)
  # Do not link Boost libraries automatically, since we explicitly link in CMake.
  add_definitions(-DBOOST_ALL_NO_LIB)
endif(MSVC)
include_directories(${Boost_INCLUDE_DIRS})


### ply (local copy in source tree) ###
file(GLOB PLY_SOURCES thirdparty/ply-0.1/ply/*.cpp)
file(GLOB PLY_HEADERS thirdparty/ply-0.1/ply/*.hpp)
list(REMOVE_ITEM PLY_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ply-0.1/ply/config.hpp)
source_group("thirdparty\\ply" FILES ${PLY_HEADERS} ${PLY_SOURCES})
include_directories(./thirdparty/ply-0.1)

### TR1 implementation (platform dependent) ###
#if(WIN32)
  # Use the Boost TR1 implementation.
  include_directories(${Boost_INCLUDE_DIRS}/boost/tr1)
#endif()


################################################################################
### library
################################################################################
add_library(io SHARED
  ${IO_SOURCES}
  ${IO_HEADERS}
  ${PLY_SOURCES}
  ${PLY_HEADERS}
)

target_link_libraries(io
  ${Boost_LIBRARIES}
)



################################################################################
### install
################################################################################
configure_file(
  ${PROJECT_SOURCE_DIR}/ioConfig.cmake.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ioConfig.cmake
  @ONLY)
configure_file(
  ${PROJECT_SOURCE_DIR}/ioConfigVersion.cmake.in
  ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ioConfigVersion.cmake
  @ONLY)
INSTALL(
  FILES ${IO_HEADERS}
  DESTINATION include/${PROJECT_NAME}-${VERSION_STR}/io)
INSTALL(
  TARGETS io
  DESTINATION lib)
INSTALL(
  FILES ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ioConfig.cmake
        ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ioConfigVersion.cmake
  DESTINATION lib/${PROJECT_NAME}-${VERSION_STR})



endif() ### multiple include guard
################################################################################
