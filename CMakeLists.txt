# ┌──────────────────────────────────────────────────────────────────┐
# │  Projects Settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cmake_minimum_required (VERSION 3.13)
project(Pluto)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Use c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif(MSVC)

# Set the install prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/" CACHE PATH "..." FORCE)
endif()

# Force binaries to go to build folder, not subfolders
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )


# ┌──────────────────────────────────────────────────────────────────┐
# │  External Dependencies                                           │
# └──────────────────────────────────────────────────────────────────┘

# swig
find_package(SWIG 3.0.12 REQUIRED)
include(${SWIG_USE_FILE})

# python
find_package (Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(SYSTEM ${Python3_INCLUDE_DIRS})

# add libraries to a list for linking
set (
    LIBRARIES
    ${Python3_LIBRARIES} 
)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Build Bridge Library                                            │
# └──────────────────────────────────────────────────────────────────┘
set (BRIDGE_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/bridge.hxx
    ${CMAKE_CURRENT_SOURCE_DIR}/bridge.cxx
)

add_library(BridgeLib SHARED ${BRIDGE_SRC})
target_link_libraries(BridgeLib PUBLIC ${LIBRARIES})
set_target_properties(BridgeLib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
install(FILES ${CMAKE_BINARY_DIR}/BridgeLib.dll DESTINATION ${CMAKE_INSTALL_PREFIX})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Build Bridge Bindings                                           │
# └──────────────────────────────────────────────────────────────────┘

# Build SWIG module 
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/bridge.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/bridge.i PROPERTY USE_TARGET_INCLUDE_DIRECTORIES TRUE)
swig_add_library(Bridge TYPE SHARED LANGUAGE python OUTFILE_DIR ${CMAKE_CURRENT_SOURCE_DIR} SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/bridge.i)
target_link_libraries(Bridge PUBLIC ${LIBRARIES} BridgeLib)

# Install
install(FILES ${CMAKE_BINARY_DIR}/Bridge.py DESTINATION ${CMAKE_INSTALL_PREFIX})

if (WIN32) 
install(FILES ${CMAKE_BINARY_DIR}/_Bridge.pyd DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_BINARY_DIR}/_Bridge.pdb DESTINATION ${CMAKE_INSTALL_PREFIX} OPTIONAL)
else()
install(FILES ${CMAKE_BINARY_DIR}/_Bridge.so DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

# ┌──────────────────────────────────────────────────────────────────┐
# │  Build Kernel Executable                                         │
# └──────────────────────────────────────────────────────────────────┘
set (KERNEL_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cxx
)

add_executable(Kernel ${KERNEL_SRC})
target_link_libraries(Kernel PUBLIC BridgeLib ${LIBRARIES} )
target_include_directories(Kernel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
install(FILES ${CMAKE_BINARY_DIR}/Kernel.exe DESTINATION ${CMAKE_INSTALL_PREFIX})

# ┌──────────────────────────────────────────────────────────────────┐
# │  Example Scripts                                                 │
# └──────────────────────────────────────────────────────────────────┘
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/A.py DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/B.py DESTINATION ${CMAKE_INSTALL_PREFIX})