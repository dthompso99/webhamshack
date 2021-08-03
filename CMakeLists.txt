cmake_minimum_required(VERSION 3.7)
include(ExternalProject)
PROJECT(web-rig)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")
SET(ENV{LD_LIBRARY_PATH} "/build/lib:$ENV{LD_LIBRARY_PATH}")
set( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

SET(CMAKE_PREFIX_PATH "/build")

find_package(hamlib REQUIRED)


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/nlohmann)
add_subdirectory(vendor/cpp-httplib-master)

LINK_DIRECTORIES(
    /build/lib
)

add_executable(${PROJECT_NAME} 
    main.cpp 
    src/controller.cpp
    src/rigctrl.cpp
    src/util.cpp
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
#target_include_directories(${PROJECT_NAME} PUBLIC ${LIBAV_INCLUDE_DIRS})

#set_compiler_flags(${PROJECT_NAME})
#set_compiler_warnings(${PROJECT_NAME})
#httplib::httplib linkable after doing find_package(httplib)
set(CMAKE_AUTOMOC ON)
target_link_libraries(${PROJECT_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    hamlib
    httplib::httplib
    pthread   
    stdc++fs
)
