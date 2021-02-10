cmake_minimum_required(VERSION 3.18)

project("Domestic Supervisor" VERSION 0.1.0)

cmake_policy(SET CMP0074 NEW)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

find_package(Boost CONFIG REQUIRED COMPONENTS iostreams headers filesystem
                                              thread program_options)
find_package(dlib CONFIG REQUIRED)
find_package(PahoMqttCpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(wt CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

if(WIN32)
  set(system_libs crypt32 rpcrt4)
else()
  set(system_libs "")
endif()

set(common_headers
    include/common/blueprint.hpp
    include/common/device.hpp
    include/common/event.hpp
    include/common/icon_button.hpp
    include/common/mapper.hpp
    include/common/mqtt.hpp
    include/common/state.hpp
    include/common/view.hpp)
set(common_srcs
    src/common/device.cpp
    src/common/state.cpp
    src/common/blueprint.cpp
    src/common/constants.cpp
    src/common/icon_button.cpp
    src/common/mqtt.cpp)
set(supervisor_srcs
    src/supervisor/main.cpp
    src/supervisor/deepnet.cpp
    src/supervisor/camera_panel.cpp
    src/supervisor/camera.cpp
    src/supervisor/domestic_supervisor.cpp
    src/supervisor/page_anchor.cpp
    src/supervisor/domestic_server.cpp
    src/supervisor/dynamic_image.cpp)
set(simulator_srcs src/simulator/main.cpp src/simulator/image_loop.cpp
                   src/simulator/domestic_simulator.cpp)

add_library(domestic-lib STATIC ${common_srcs})
set_target_properties(
  domestic-lib
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             PUBLIC_HEADER "${common_headers}")
target_compile_features(domestic-lib PRIVATE cxx_std_20)
target_compile_options(domestic-lib PRIVATE -Wall)
target_include_directories(
  domestic-lib
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/common>
         $<INSTALL_INTERFACE:include/common>
  PRIVATE $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
target_link_libraries(
  domestic-lib
  PUBLIC ${Boost_LIBRARIES}
          ${system_libs}
          Wt::Wt
          dlib::dlib
          PahoMqttCpp::paho-mqttpp3-static
          spdlog::spdlog
          opencv_core
          opencv_videoio
          opencv_imgcodecs)

add_executable(domestic-supervisor ${supervisor_srcs})
set_target_properties(
  domestic-supervisor
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             )
target_compile_features(domestic-supervisor PRIVATE cxx_std_20)
target_compile_options(domestic-supervisor PRIVATE -Wall)
target_include_directories(
  domestic-supervisor
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/supervisor>)
target_link_libraries(domestic-supervisor PRIVATE Wt::HTTP domestic-lib)

add_executable(domestic-simulator ${simulator_srcs})
set_target_properties(
  domestic-simulator
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON)
target_compile_features(domestic-simulator PRIVATE cxx_std_20)
target_compile_options(domestic-simulator PRIVATE -Wall)
target_include_directories(
  domestic-simulator
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/simulator>)
target_link_libraries(domestic-simulator PRIVATE Wt::HTTP domestic-lib)

if(ipo_supported AND NOT WIN32)
  message(STATUS "IPO/LTO enabled")
  set_target_properties(domestic-lib
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_target_properties(domestic-supervisor
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_target_properties(domestic-simulator
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO/LTO not supported! <${error}>")
endif()

install(TARGETS domestic-lib
                EXPORT domestic-lib
                ARCHIVE
                PUBLIC_HEADER)
install(EXPORT domestic-lib
               NAMESPACE domestic-supervisor::
               FILE domestic-lib-config.cmake
               DESTINATION lib/cmake/domestic-supervisor)
install(TARGETS domestic-supervisor domestic-simulator)
install(DIRECTORY assets TYPE BIN)
if(WIN32)
  foreach(DLL $ENV{EXTRA_DLLS})
    install(FILES ${DLL} TYPE BIN)
  endforeach()
endif()
