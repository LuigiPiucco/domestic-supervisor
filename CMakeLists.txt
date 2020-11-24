cmake_minimum_required(VERSION 3.18)

project("Domestic Supervisor" VERSION 0.1.0)

cmake_policy(SET CMP0074 NEW)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT error)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.74 CONFIG REQUIRED COMPONENTS iostreams headers filesystem
                                                   thread program_options)
find_package(dlib CONFIG REQUIRED)
find_package(PahoMqttCpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(wt CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

set(common_srcs
    src/common/device.cpp
    src/common/state.cpp
    src/common/view.cpp
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

if(WIN32)
  set(system_libs ws2_32 crypt32 rpcrt4)
else()
  set(system_libs "")
endif()

add_executable(domestic-supervisor ${supervisor_srcs} ${common_srcs})
set_target_properties(
  domestic-supervisor
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON)
target_compile_features(domestic-supervisor PRIVATE cxx_std_20)
target_compile_options(domestic-supervisor PRIVATE -Wall)
target_include_directories(
  domestic-supervisor
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/supervisor>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/common>
          $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>)
target_link_libraries(
  domestic-supervisor
  PRIVATE ${Boost_LIBRARIES}
          Wt::Wt
          Wt::HTTP
          dlib::dlib
          PahoMqttCpp::paho-mqttpp3-static
          spdlog::spdlog
          opencv_core
          opencv_imgcodecs
          ${system_libs})

add_executable(domestic-simulator ${simulator_srcs} ${common_srcs})
set_target_properties(
  domestic-simulator
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON)
target_compile_features(domestic-simulator PRIVATE cxx_std_20)
target_compile_options(domestic-simulator PRIVATE -Wall)
target_include_directories(
  domestic-simulator
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/simulator>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/common>)
target_link_libraries(
  domestic-simulator
  PRIVATE ${Boost_LIBRARIES}
          Wt::Wt
          Wt::HTTP
          PahoMqttCpp::paho-mqttpp3-static
          spdlog::spdlog
          opencv_core
          opencv_imgcodecs
          opencv_imgproc
          opencv_videoio
          ${system_libs})

if(ipo_supported)
  message(STATUS "IPO/LTO enabled")
  set_target_properties(domestic-supervisor
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_target_properties(domestic-simulator
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO/LTO not supported! <${error}>")
endif()

install(TARGETS domestic-supervisor domestic-simulator
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY assets TYPE BIN)
if(WIN32)
  install(FILES $ENV{MCFGTHREADS_DLL} TYPE BIN)
endif()
