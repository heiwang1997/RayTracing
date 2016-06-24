cmake_minimum_required(VERSION 2.8)
project(RayTracing)

set(SOURCE_FILES basic.cpp        image.h        objloader.h           primitive.cpp
basic.h          kdtritree.cpp  photonmap.cpp         primitive.h
camera.cpp       kdtritree.h    photonmap.h           raytracer.cpp
camera.h         light.cpp      photontracer.cpp      raytracer.h
default.h        light.h        photontracer.h        scene.cpp
hitpointmap.cpp  main.cpp       photonvisualizer.cpp
hitpointmap.h    objloader.cpp  photonvisualizer.h scene.h stb_image_write.h tiny_obj_loader.h stb_image.h)
find_package (Threads)
add_executable(RayTracing ${SOURCE_FILES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")
set_property(TARGET RayTracing PROPERTY COMPILE_OPTIONS "-pthread")
set_property(TARGET RayTracing PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
TARGET_LINK_LIBRARIES(RayTracing ${CMAKE_THREAD_LIBS_INIT})
