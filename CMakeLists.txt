cmake_minimum_required(VERSION 3.16)
project(Nmsdk_PulseLib LANGUAGES CXX)

set(TARGET_NAME Nmsdk-PulseLib.qt)

add_library(${TARGET_NAME} STATIC)

target_include_directories(${TARGET_NAME}
  PUBLIC
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core
)

file(GLOB PULSE_SOURCES
  ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core/*.cpp
)

# Исключаем файлы с ошибками компиляции
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/.*Gui.*\\.cpp$")
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NLifeNet\\.cpp$")
list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NResultDecoder\\.cpp$")
# Не исключаем NPCAClassifier.cpp — он требуется для NMSDK::NPCAClassifier
# Не исключаем NPulseLibrary.cpp — он определяет глобальный объект PulseLibrary

# Исключаем NOdeSolver.cpp, если RDK_USE_ODESOLVER выключен
if(NOT RDK_USE_ODESOLVER)
  list(FILTER PULSE_SOURCES EXCLUDE REGEX "/Libraries/Nmsdk-PulseLib/Core/NOdeSolver\\.cpp$")
endif()

target_sources(${TARGET_NAME} PRIVATE ${PULSE_SOURCES})

target_compile_definitions(${TARGET_NAME} PUBLIC RDK_QT RDK_LIB_CVS_REVISION=0)

# Передаём RDK_USE_ODESOLVER в компилятор, если он включён
if(RDK_USE_ODESOLVER)
  target_compile_definitions(${TARGET_NAME} PUBLIC RDK_USE_ODESOLVER)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
target_link_libraries(${TARGET_NAME} PUBLIC Qt5::Core Threads::Threads rdk.static.qt Rdk-BasicLib.qt Rdk-CvBasicLib.qt)


