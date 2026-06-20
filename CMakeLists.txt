cmake_minimum_required(VERSION 3.28)
project(wxChaos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use static linking if needed
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# ------
# SFML -
# ------
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        SYSTEM)
FetchContent_MakeAvailable(SFML)

# -----------
# wxWidgets -
# -----------
find_package(wxWidgets REQUIRED COMPONENTS core base adv html stc richtext xml)
message(STATUS "wxWidgets libraries: ${wxWidgets_LIBRARIES}")

include(${wxWidgets_USE_FILE})

find_path(MPFR_INCLUDE_DIR mpfr.h REQUIRED)
find_library(MPFR_LIBRARY NAMES mpfr REQUIRED)
find_library(GMP_LIBRARY NAMES gmp REQUIRED)

# -------------
# AngelScript -
# -------------

find_package(Threads REQUIRED)
add_subdirectory(
        ${CMAKE_SOURCE_DIR}/third_party/angelscript/sdk/angelscript/projects/cmake
        ${CMAKE_BINARY_DIR}/angelscript-build
)

target_sources(angelscript PRIVATE
        ${CMAKE_SOURCE_DIR}/third_party/angelscript/sdk/add_on/scriptstdstring/scriptstdstring.cpp
        ${CMAKE_SOURCE_DIR}/third_party/angelscript/sdk/add_on/scriptstdstring/scriptstdstring_utils.cpp
)

# -------------------------------------------------------------------
# Source Files (trimmed here for readability)
# -------------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
        wxChaos/core/*.cpp
        wxChaos/gui/*.cpp
        wxChaos/utils/*.cpp
        wxChaos/scripting/*.cpp
        third_party/muParserX/*.cpp
        third_party/wxMathPlot/*.cpp
)

add_executable(wxChaos WIN32 ${SRC_FILES}
        wxChaos/core/ThreadWatchdog.h
        wxChaos/App.cpp
)

# -------------------------------------------------------------------
# Include Directories (use target-based)
# -------------------------------------------------------------------
target_include_directories(wxChaos PRIVATE
        ${wxWidgets_INCLUDE_DIRS}
        ${MPFR_INCLUDE_DIR}
        ${SFML_INCLUDE_DIRS}
        wxChaos/core
        wxChaos/gui
        wxChaos/utils
        wxChaos/scripting
        third_party/angelscript/sdk/angelscript/include
        third_party/angelscript/sdk/add_on/scriptstdstring
        third_party/muParserX
        third_party/wxMathPlot
)

# -------------------------------------------------------------------
# Definitions
# -------------------------------------------------------------------
target_compile_definitions(wxChaos PRIVATE
        SFML_STATIC
        WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
        WXCHAOS_BUILD_TYPE="$<CONFIG>"
        $<$<CONFIG:Debug>:__WXDEBUG__>
        _HAS_STD_BYTE=0    # turns off the enum class std::byte in MSVC’s STL
)

# -------------------------------------------------------------------
# Linking
# -------------------------------------------------------------------
target_link_libraries(wxChaos PRIVATE
        ${wxWidgets_LIBRARIES}
        sfml-graphics
        sfml-window
        sfml-system
        sfml-main
        angelscript
        ${MPFR_LIBRARY}
        ${GMP_LIBRARY}
        Threads::Threads          # pthreads
        winmm comctl32 rpcrt4 wsock32 opengl32
)

# -------------------------------------------------------------------
# Optional MSVC Settings
# -------------------------------------------------------------------
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_link_options(wxChaos PRIVATE "/NODEFAULTLIB:libcmt.lib")
    target_compile_options(wxChaos PRIVATE
            $<$<CONFIG:Release>:/fp:fast>
            $<$<CONFIG:Release>:/Ot>
    )
    set_property(TARGET wxChaos PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

# -------------------------------------------------------------------
# Post-Build: Copy resources
# -------------------------------------------------------------------
add_custom_command(
        TARGET wxChaos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/app_resources/"
        "$<TARGET_FILE_DIR:wxChaos>/"
        COMMENT "Copying application resources..."
)
