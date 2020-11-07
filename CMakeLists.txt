cmake_minimum_required(VERSION 3.18)

project("Domestic Supervisor" VERSION 0.1.0)

include(CheckIPOSupported)

check_ipo_supported(RESULT ipo_supported OUTPUT error)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
string(APPEND CMAKE_CXX_FLAGS "-march=native -Wall")

find_package(OpenCV CONFIG REQUIRED)
find_package(dlib CONFIG REQUIRED)
find_package(wt CONFIG REQUIRED)
find_package(PahoMqttCpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(common_srcs src/common/device.cpp src/common/state.cpp src/common/view.cpp
                src/common/blueprint.cpp)

add_executable(domestic-supervisor src/supervisor/main.cpp ${common_srcs})
set_target_properties(
  domestic-supervisor
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON)
target_include_directories(
  domestic-supervisor
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/supervisor>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/common>)
target_link_libraries(
  domestic-supervisor
  PRIVATE Wt::Wt
          Wt::HTTP
          dlib::dlib
          PahoMqttCpp::paho-mqttpp3
          spdlog::spdlog
          opencv_core
          opencv_imgcodecs)
target_compile_options(domestic-supervisor PRIVATE -nostdinc)
target_link_options(domestic-supervisor PRIVATE -stdlib=libc++ -fuse-ld=lld)

add_executable(domestic-simulator src/simulator/main.cpp ${common_srcs})
set_target_properties(
  domestic-simulator
  PROPERTIES CXX_EXTENSIONS OFF
             CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON)
target_include_directories(
  domestic-simulator
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/simulator>
          $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/common>)
target_link_libraries(
  domestic-simulator
  PRIVATE Wt::Wt
          Wt::HTTP
          PahoMqttCpp::paho-mqttpp3
          spdlog::spdlog
          opencv_core
          opencv_imgcodecs
          opencv_imgproc
          opencv_videoio)
target_compile_options(domestic-simulator PRIVATE -nostdinc)
target_link_options(domestic-simulator PRIVATE -stdlib=libc++ -fuse-ld=lld)

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
configure_file(${CMAKE_CURRENT_LIST_DIR}/compile_flags.txt
               ${CMAKE_BINARY_DIR}/compile_flags.txt COPY_ONLY)
