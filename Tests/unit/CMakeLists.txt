# Nmsdk-PulseLib Unit Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for PulseLib tests
add_executable(NmsdkPulseLibTests
    TestPulseComponents.cpp
    TestPulseComponentsWithoutCreateSamples.cpp
    TestPulseLibRegistration.cpp
    TestPulseNeuronVariants.cpp
    TestPulseSynapseVariants.cpp
    TestPulseChannelVariants.cpp
    TestPulseGeneratorVariants.cpp
    TestPulseLTZoneVariants.cpp
    TestPulseMembraneVariants.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(NmsdkPulseLibTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core
    ${CMAKE_SOURCE_DIR}/Tests
)

target_link_libraries(NmsdkPulseLibTests PRIVATE
    rdk.static.qt
    Nmsdk-PulseLib.qt
    Rdk-CvBasicLib.qt
    Rdk-HardwareLib.qt
    Nmsdk-MotionControlLib.qt
    TestHelpers
    GTest::gtest
    GTest::gtest_main
    glog::glog
    Threads::Threads
)

target_compile_features(NmsdkPulseLibTests PRIVATE cxx_std_20)
target_compile_definitions(NmsdkPulseLibTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME NmsdkPulseLibTests COMMAND NmsdkPulseLibTests)
