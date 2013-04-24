cmake_minimum_required(VERSION 2.6) # Pour éviter un warning au lancement de CMake, on doit préciser la version minimale

# Nom du projet
project(head_pose_estimation_NB)

include_directories(
        /usr/local/include/opencv2
        /usr/local/include
        /usr/include/opencv2
	./include/
)

link_directories(
        /usr/local/lib
	/usr/lib    
)

file(
    GLOB_RECURSE
    source_files
    src/*
    include/*
)

set(CMAKE_BUILD_TYPE Release)
set(EXECUTABLE_OUTPUT_PATH ../bin/)
set(CMAKE_CXX_FLAGS "-std=c++0x -g -ggdb -fopenmp")#active C++ 2011 standard

add_library(
    random_forest
    SHARED
    ${source_files}
)

#find_package (Threads)
find_package( OpenCV REQUIRED )


# Déclaration de l'exécutable
add_executable(
    head_pose_estimation_NB
    bin/src/head_pose_estimation_NB.cpp
    ${source_files}
)

target_link_libraries(
    head_pose_estimation_NB
		random_forest
		${OpenCV_LIBS}
		#${CMAKE_THREAD_LIBS_INIT}
	#libs de boost
		boost_filesystem
		boost_system
	#libs de tinyxml
		tinyxml
)


