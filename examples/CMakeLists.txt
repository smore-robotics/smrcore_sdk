cmake_minimum_required(VERSION 3.16)
project(smrcore_sdk_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point -DCMAKE_PREFIX_PATH at the extracted SDK directory.
# The SDK package provides lib/cmake/smrcore_sdk/smrcore_sdkConfig.cmake.
find_package(smrcore_sdk REQUIRED)

# Examples are grouped by topic. Each entry is "<dir>/<name>"; the executable is
# named "<dir>_<name>" so targets stay unique across directories.
set(SMRCORE_SDK_EXAMPLES
    basics/connect
    basics/read_state
    basics/error_recovery
    motion/movej
    motion/movep
    motion/movel
    motion/movec
    motion/motion_api
    motion/move_path
    motion/async_motion
    motion/servoj
    motion/servop
    config/config_limits
    config/waypoints
    config/payload
    compliance/cartesian_impedance
    compliance/fd_cartesian_admittance
)

foreach(example ${SMRCORE_SDK_EXAMPLES})
    string(REPLACE "/" "_" target_name ${example})
    add_executable(${target_name} ${example}.cpp)
    target_link_libraries(${target_name} PRIVATE smrcore::sdk)
endforeach()
