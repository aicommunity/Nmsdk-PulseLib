# Nmsdk-PulseLib Integration Tests CMake Configuration
cmake_minimum_required(VERSION 3.16)

# Find Google Test
find_package(GTest REQUIRED)

# Create test executable for PulseLib integration tests
add_executable(NmsdkPulseLibIntegrationTests
    TestPulseLibIntegration.cpp
    ${CMAKE_SOURCE_DIR}/Libraries/Libraries_con.cpp
)

target_include_directories(NmsdkPulseLibIntegrationTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Rdk/Core
    ${CMAKE_SOURCE_DIR}/Rdk/Deploy/Include
    ${CMAKE_SOURCE_DIR}/Libraries/Nmsdk-PulseLib/Core
    ${CMAKE_SOURCE_DIR}/Tests
)

target_link_libraries(NmsdkPulseLibIntegrationTests PRIVATE
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

target_compile_features(NmsdkPulseLibIntegrationTests PRIVATE cxx_std_20)
target_compile_definitions(NmsdkPulseLibIntegrationTests PRIVATE RDK_QT RDK_LIB_CVS_REVISION=0)

# Register tests
add_test(NAME NmsdkPulseLibIntegrationTests COMMAND NmsdkPulseLibIntegrationTests)
