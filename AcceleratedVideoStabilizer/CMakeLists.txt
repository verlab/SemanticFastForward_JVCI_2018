cmake_minimum_required(VERSION 2.8.12)

project(VideoStabilization)

#########################################################
# FIND OPENCV
#########################################################
find_package(OpenCV 2 REQUIRED)
if(OpenCV_FOUND)
	include_directories(${OpenCV_INCLUDE_DIRS})
	message("===================================================")
	message(STATUS "OpenCV library status:")
	message(STATUS "    version: ${OpenCV_VERSION}")
	message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
	message(STATUS "    libraries: ${OpenCV_LIBS}")
	message("===================================================")
endif()
########################################################

#########################################################
# FIND OPENMP
#########################################################
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

########################################################

#########################################################
# FIND BOOST
#########################################################
find_package(Boost REQUIRED COMPONENTS filesystem system)
if(Boost_FOUND)
  	include_directories(${Boost_INCLUDE_DIRS})
  	message("===================================================")
	message(STATUS "Boost library status:")
	message(STATUS "    version: ${Boost_VERSION}")
	message(STATUS "    include path: ${Boost_INCLUDE_DIRS}")
	message(STATUS "    libraries: ${Boost_LIBRARIES}")
	message("===================================================")
endif()
########################################################

link_directories(
	/usr/local/lib/
)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR} 
	${CMAKE_CURRENT_SOURCE_DIR}/definitions 
	${CMAKE_CURRENT_SOURCE_DIR}/headers 
	${CMAKE_CURRENT_SOURCE_DIR}/executables
)

set (HEADER_FILES 
    definitions/experiments.h 
    definitions/experiment_struct.h
    definitions/define.h
    executables/execute_commands.h
    headers/homography.h
    headers/sequence_processing.h
    headers/file_operations.h
    headers/master_frames.h
    headers/image_reconstruction.h
    headers/line_and_point_operations.h
    headers/message_handler.h
    headers/error_messages.h
)

set (SOURCES
    src/main.cpp
    src/homography.cpp 
    src/sequence_processing.cpp
    src/file_operations.cpp 
    src/master_frames.cpp 
    src/image_reconstruction.cpp 
    src/line_and_point_operations.cpp 
    src/message_handler.cpp 
)

set (LIBS
	${OpenCV_LIBS}
	${ARMADILLO_LIBRARIES}
	${Boost_LIBRARIES}
    armadillo
)

add_executable(EgoStabilizer ${SOURCES} ${HEADER_FILES})

target_link_libraries(EgoStabilizer ${LIBS} )
