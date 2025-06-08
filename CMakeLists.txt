cmake_minimum_required(VERSION 3.15)
project(wxChaos)
set(CMAKE_CXX_STANDARD 17)

find_package(wxWidgets REQUIRED COMPONENTS core base)
find_package(SFML 2.6 REQUIRED COMPONENTS graphics window system)

include(${wxWidgets_USE_FILE})

file(GLOB SRC "wxChaos/*.cpp" "wxChaos/wxMathPlot/*.cpp")
file(GLOB HDR "wxChaos/*.h" "wxChaos/wxMathPlot/*.h")
add_executable(wxChaos ${SRC} ${HDR})

target_include_directories(wxChaos PRIVATE wxChaos)

target_link_libraries(wxChaos PRIVATE ${wxWidgets_LIBRARIES} sfml-graphics sfml-window sfml-system)
