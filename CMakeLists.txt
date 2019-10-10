# Version check
cmake_minimum_required (VERSION 3.8)

if(NOT "${CMAKE_CXX_STANDARD}")
	set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

# Project
project(bluetooth_test)

# Add files
file(GLOB sources "main.cpp"
				  "bluetooth/rfcomm_server.cpp"
				  "bluetooth/bluez/device.cpp"
				  "bluetooth/bluez/adapter.cpp"
				  "bluetooth/bluez/connection_manager.cpp"
				  "bluetooth/bluez/reactor.cpp"
)

# Add exec
add_executable(bluetooth_test ${sources})

# Linking & Includes
target_include_directories(${PROJECT_NAME} PRIVATE "../dbus-mockery/include" "../dbus-mockery-system/include")

include(FindPkgConfig)

pkg_check_modules(SYSTEMD "libsystemd")

find_library(LBLUETOOTH NAMES bluetooth)
find_library(LDBUSMOCK NAMES dbus-mockery PATHS "../dbus-mockery/build")

message("Following paths for the libraries were found:")
message("\tbluetooth: " ${LBLUETOOTH})
message("\tdbus-mock: " ${LDBUSMOCK})

target_link_libraries(
	bluetooth_test
	${LBLUETOOTH}
	${LDBUSMOCK}
	${SYSTEMD_LIBRARIES}
	-lpthread
)

# Compiler Options
set(DEBUG_OPTIONS -fexceptions -g -Wall -pedantic-errors -pedantic)
set(RELEASE_OPTIONS -fexceptions -O3 -Wall -pedantic-errors -pedantic)
target_compile_options(bluetooth_test PUBLIC "$<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>")
target_compile_options(bluetooth_test PUBLIC "$<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>")
