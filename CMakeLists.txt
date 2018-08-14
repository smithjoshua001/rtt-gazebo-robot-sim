cmake_minimum_required(VERSION 2.8.3)
project(rtt-gazebo-robot-sim)

function(ExtractLibraries outputVar libraryHints)
  message(STATUS "EXTRACTING: ${ARGN}")
	foreach(LIB ${ARGN})
		message(STATUS "LIB: ${LIB}")
		find_library(TMP ${LIB} HINTS ${libraryHints})
		set( ${outputVar} ${${outputVar}} ${TMP})
		unset(TMP CACHE)
	endforeach(LIB)
  set(${outputVar} ${${outputVar}} PARENT_SCOPE)
endfunction()

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(gazebo REQUIRED)
if(GAZEBO_VERSION)
  if(${GAZEBO_VERSION} GREATER 6)
      add_definitions(-DGAZEBO_GREATER_6)
  endif()
endif()

find_package(OROCOS-RTT REQUIRED COMPONENTS rtt-scripting rtt-typekit)
if (NOT OROCOS-RTT_FOUND)
  message (FATAL_ERROR "\nCould not find Orocos-RTT. Please use the shell command\n 'source orocos_toolchain/env.sh' and then run cmake again.")
endif()

include(${OROCOS-RTT_USE_FILE_PATH}/UseOROCOS-RTT.cmake )

set (Eigen_INCLUDE_DIRS "/usr/include/eigen3")
find_package(orocos_kdl REQUIRED)
#find_package(Boost COMPONENTS thread REQUIRED)
find_package(srdfdom_advr REQUIRED)
find_package(XBotCoreModel REQUIRED)
link_directories(${XBotCoreModel_LIBRARY_DIRS})
message(STATUS "XBotCoreModel_LIBRARY_DIRS: ${XBotCoreModel_LIBRARY_DIRS}")
find_library(TMP_XBOT NAMES ${XBotCoreModel_LIBRARIES} HINTS ${XBotCoreModel_LIBRARY_DIRS})
message(STATUS "XBotCoreModel_LIBRARIES: ${TMP_XBOT}")

find_package(RST-RT REQUIRED)
message(STATUS "RST-RT version: ${RST-RT_VERSION}")
include_directories(BEFORE SYSTEM ${RST-RT_INCLUDE_DIRS})
add_definitions(${RST-RT_CFLAGS})
link_directories(${RST-RT_LIBRARY_DIR} ${RST-RT_LIBRARY_DIRS})

find_package(PkgConfig REQUIRED)
pkg_search_module(TINYXML2 REQUIRED tinyxml2>=5.0.0)


ExtractLibraries(TINYXML_LIBRARIES ${TINYXML2_LIBRARY_DIRS} ${TINYXML2_LIBRARIES})
message(STATUS "TINYXML2 LIBS: ${TINYXML2_LIBRARIES}")



message(STATUS "TINYXML_VERSION: ${TINYXML2_VERSION}")
include_directories(BEFORE SYSTEM ${TINYXML2_INCLUDE_DIRS})
link_directories(${TINYXML2_LIBRARY_DIRS})
message(STATUS "TINYXML_LIBRARIES: ${TINYXML_LIBRARIES}")
message(STATUS "TINYXML_LIBRARY_DIR: ${TINYXML2_LIBRARY_DIR}")
message(STATUS "TINYXML_INCLUDE_DIR: ${TINYXML2_INCLUDE_DIRS}")
message(STATUS "TINYXML_LIBRARY: ${TINYXML2_LIBRARY}")

include_directories(
    include
    ${Boost_INCLUDE_DIR}
    ${Eigen_INCLUDE_DIRS}
    ${orocos_kdl_INCLUDE_DIRS}
    ${USE_OROCOS_INCLUDE_DIRS}
    ${OROCOS-RTT_INCLUDE_DIRS}
    ${GAZEBO_INCLUDE_DIRS}
    ${RST-RT_INCLUDE_DIRS}
    ${TINYXML2_INCLUDE_DIRS}
    ${srdfdom_advr_INCLUDE_DIRS}
    ${XBotCoreModel_INCLUDE_DIRS}
)

file(GLOB_RECURSE rtt_gazebo_robot_sim_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include" *.h)

ADD_LIBRARY(gain_parser SHARED src/parser.cpp)
#set_property(TARGET gain_parser APPEND PROPERTY COMPILE_DEFINITIONS RTT_COMPONENT)
TARGET_LINK_LIBRARIES(gain_parser ${TINYXML_LIBRARIES})

orocos_component(${PROJECT_NAME} src/rtt-gazebo-robot-sim.cpp src/rtt-gazebo-robot-sim_hooks.cpp src/kinematic_chain.cpp
 src/force_torque_sensor.cpp ${rtt_gazebo_robot_sim_INCLUDES})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS RTT_COMPONENT)

target_link_libraries(${PROJECT_NAME}
   gain_parser
   ${USE_OROCOS_LIBRARIES}
   ${orocos_kdl_LIBRARIES}
   ${GAZEBO_LIBRARIES}
   ${OROCOS-RTT_LIBRARIES}
   ${Boost_LIBRARIES}
   ${RST-RT_LIBRARIES}
   ${TINYXML_LIBRARIES}
   ${srdfdom_advr_LIBRARIES}
   ${TMP_XBOT}
)

install (TARGETS gain_parser
         LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

orocos_generate_package(INCLUDE_DIRS include)

#######################
# Add Testing target  #
#######################
enable_testing()
add_subdirectory(tests)
