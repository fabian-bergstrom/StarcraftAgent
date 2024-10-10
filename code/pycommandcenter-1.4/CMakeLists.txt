cmake_minimum_required (VERSION 3.5)
project (starcraft-python-api)

# Build with C++14 support, required by sc2api
set(CMAKE_CXX_STANDARD 14)

# Allow creating filters for projects in visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Disable building for examples in the sc2api submodule
set(BUILD_API_EXAMPLES OFF CACHE INTERNAL "" FORCE)

# Disable building for tests in the sc2api submodule
set(BUILD_API_TESTS OFF CACHE INTERNAL "" FORCE)

add_subdirectory(lib/pybind11)
add_subdirectory(lib/cpp-sc2)
add_subdirectory(src)
add_subdirectory(python-api-src)

# Hack to make compile, these flags are otherwise set to give errors on warnings
if (MSVC)
    set_target_properties(sc2api PROPERTIES COMPILE_FLAGS "/W4")
    set_target_properties(sc2lib PROPERTIES COMPILE_FLAGS "/W4")
    set_target_properties(sc2renderer PROPERTIES COMPILE_FLAGS "/W4")
    set_target_properties(sc2utils PROPERTIES COMPILE_FLAGS "/W3")
    set_target_properties(sc2protocol PROPERTIES COMPILE_FLAGS "/W0")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CommandCenter)
endif (MSVC)

if(UNIX AND NOT APPLE)
    set_target_properties(libprotobuf PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(sc2api PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(sc2lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(sc2renderer PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(sc2utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(sc2protocol PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(civetweb-c-library PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
