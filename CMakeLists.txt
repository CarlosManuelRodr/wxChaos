cmake_minimum_required(VERSION 3.28)
project(wxChaos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use static linking if needed
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# -------------------------------------------------------------------
# SFML - Modern fetch with FetchContent
# -------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        SYSTEM)
FetchContent_MakeAvailable(SFML)

# -------------------------------------------------------------------
# wxWidgets - Use find_package properly
# -------------------------------------------------------------------
find_package(wxWidgets REQUIRED COMPONENTS core base adv html stc richtext xml)
message(STATUS "wxWidgets libraries: ${wxWidgets_LIBRARIES}")

include(${wxWidgets_USE_FILE})

# -------------------------------------------------------------------
# Source Files (trimmed here for readability)
# -------------------------------------------------------------------
file(GLOB_RECURSE SRC_FILES
        wxChaos/*.cpp
        wxChaos/angelscript/**/*.cpp
        wxChaos/muParserX/*.cpp
        wxChaos/wxMathPlot/*.cpp
)

add_executable(wxChaos WIN32 ${SRC_FILES})

# -------------------------------------------------------------------
# Include Directories (use target-based)
# -------------------------------------------------------------------
target_include_directories(wxChaos PRIVATE
        ${wxWidgets_INCLUDE_DIRS}
        ${SFML_INCLUDE_DIRS}
)

# -------------------------------------------------------------------
# Definitions
# -------------------------------------------------------------------
target_compile_definitions(wxChaos PRIVATE
        SFML_STATIC
        WIN32 _WINDOWS __WXMSW__ UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
        $<$<CONFIG:Debug>:__WXDEBUG__>
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
        "${CMAKE_SOURCE_DIR}/app_resources/English"
        "$<TARGET_FILE_DIR:wxChaos>/English"
        COMMENT "Copying application resources..."
)

# -------------------------------------------------------------------
# Optional: OpenAL DLL for SFML on Windows
# -------------------------------------------------------------------
if(WIN32)
    add_custom_command(
            TARGET wxChaos
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll"
            "$<TARGET_FILE_DIR:wxChaos>"
            COMMENT "Copying OpenAL DLL"
    )
endif()
