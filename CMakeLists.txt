cmake_minimum_required(VERSION 3.16)
project(Nmsdk_PulseLib LANGUAGES CXX)

option(NMSDK_PULSELIB_BUILD_CORE_ONLY
  "Build PulseLib core without Qt-dependent GUI helpers"
  OFF)

set(TARGET_CORE_NAME Nmsdk-PulseLib.core)
set(TARGET_QT_NAME Nmsdk-PulseLib.qt)

add_library(${TARGET_CORE_NAME} STATIC)
add_library(${TARGET_QT_NAME} INTERFACE)

target_include_directories(${TARGET_CORE_NAME}
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core
)

file(GLOB PULSE_SOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core/*.cpp
)

# Exclude files with known compilation errors
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/.*Gui.*\\.cpp$")
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NLifeNet\\.cpp$")
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NResultDecoder\\.cpp$")
# Do not exclude NPCAClassifier.cpp — it is required for NMSDK::NPCAClassifier
# Do not exclude NPulseLibrary.cpp — it defines the global PulseLibrary object

# Exclude NOdeSolver.cpp when RDK_USE_ODESOLVER is disabled
if(NOT RDK_USE_ODESOLVER)
  list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NOdeSolver\\.cpp$")
endif()

target_sources(${TARGET_CORE_NAME} PRIVATE ${PULSE_SOURCES})

if(NMSDK_PULSELIB_BUILD_CORE_ONLY)
  target_compile_definitions(${TARGET_CORE_NAME} PUBLIC NMSDK_PULSELIB_CORE_ONLY)
endif()

target_compile_definitions(${TARGET_CORE_NAME} PUBLIC RDK_QT RDK_LIB_CVS_REVISION=0)

# Pass RDK_USE_ODESOLVER to the compiler when it is enabled
if(RDK_USE_ODESOLVER)
  target_compile_definitions(${TARGET_CORE_NAME} PUBLIC RDK_USE_ODESOLVER)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/SelectQt.cmake)

target_link_libraries(${TARGET_CORE_NAME} PUBLIC
  Threads::Threads
  rdk.static.qt
  Rdk-BasicLib.qt
  Rdk-CvBasicLib.qt
)

target_link_libraries(${TARGET_QT_NAME} INTERFACE
  ${TARGET_CORE_NAME}
)

if(NOT NMSDK_PULSELIB_BUILD_CORE_ONLY)
  target_link_libraries(${TARGET_QT_NAME} INTERFACE
    Nmsdk::Qt::Core
  )
endif()


