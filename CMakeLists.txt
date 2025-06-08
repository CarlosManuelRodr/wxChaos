cmake_minimum_required(VERSION 3.16)
project(wxChaos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME SFML
    GITHUB_REPOSITORY SFML/SFML
    GIT_TAG 2.6.2
)
CPMAddPackage(
    NAME wxwidgets
    GITHUB_REPOSITORY wxWidgets/wxWidgets
    GIT_TAG v3.2.4
    OPTIONS "wxBUILD_SHARED=OFF" "wxBUILD_SAMPLES=OFF" "wxBUILD_TESTS=OFF"
)

add_subdirectory(wxChaos)
