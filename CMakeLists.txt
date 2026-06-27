cmake_minimum_required(VERSION 3.28)
project(wxChaos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use static linking if needed
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# -----------
# wxWidgets -
# -----------
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base adv html stc richtext xml)
set(WXCHAOS_WX_LIBRARIES
        wx::core
        wx::base
        wx::adv
        wx::html
        wx::stc
        wx::richtext
        wx::xml
)

find_package(doctest CONFIG REQUIRED)
find_package(SFML 2.6.2 CONFIG REQUIRED COMPONENTS graphics window system main)

# SymEngine's exported CMake config currently declares compatibility with
# CMake 2.8.12, which CMake 4 rejects. Keep this workaround local to package
# discovery instead of patching generated vcpkg files.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

find_package(SymEngine CONFIG REQUIRED)

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
        wxChaos/App.cpp
)

# -------------------------------------------------------------------
# Include Directories (use target-based)
# -------------------------------------------------------------------
target_include_directories(wxChaos PRIVATE
        ${SYMENGINE_INCLUDE_DIRS}
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
        WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
        WXCHAOS_BUILD_TYPE="$<CONFIG>"
        $<$<CONFIG:Debug>:__WXDEBUG__>
        _HAS_STD_BYTE=0    # turns off the enum class std::byte in MSVC’s STL
)

# -------------------------------------------------------------------
# Linking
# -------------------------------------------------------------------
target_link_libraries(wxChaos PRIVATE
        ${WXCHAOS_WX_LIBRARIES}
        sfml-graphics
        sfml-window
        sfml-system
        sfml-main
        angelscript
        ${SYMENGINE_LIBRARIES}
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

include(CTest)

if(BUILD_TESTING)
    add_executable(wxChaosCoreTests
            tests/core/CoreTestsMain.cpp
            tests/core/FractalUtilitiesTests.cpp
            tests/core/RenderingAlgorithmTests.cpp
            tests/core/geometry/GeometryTests.cpp
            tests/core/rendering/RenderingTests.cpp
            tests/core/UserFormulaSymbolicCompilerTests.cpp
            tests/core/NewtonRootRegistryTests.cpp
            wxChaos/core/UserFormulaSymbolicCompiler.cpp
            wxChaos/core/renderers/utils/NewtonRootRegistry.cpp
            wxChaos/core/coloring/PaletteMapping.cpp
            wxChaos/core/coloring/RenderingAlgorithm.cpp
            wxChaos/core/geometry/Rect.cpp
            wxChaos/core/geometry/Vector2Double.cpp
            wxChaos/core/rendering/RenderJob.cpp
            wxChaos/core/rendering/RenderRegion.cpp
            wxChaos/utils/TextUtils.cpp
    )

    target_include_directories(wxChaosCoreTests PRIVATE
            ${SYMENGINE_INCLUDE_DIRS}
            wxChaos/core
            wxChaos/utils
    )

    target_link_libraries(wxChaosCoreTests PRIVATE
            doctest::doctest
            ${WXCHAOS_WX_LIBRARIES}
            ${SYMENGINE_LIBRARIES}
    )

    if(MSVC)
        target_compile_definitions(wxChaosCoreTests PRIVATE
                WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
                $<$<CONFIG:Debug>:__WXDEBUG__>
                _HAS_STD_BYTE=0
        )
    endif()

    add_test(NAME wxChaosCoreTests COMMAND wxChaosCoreTests)

    add_executable(wxChaosGuiTests
            tests/gui/GuiTestsMain.cpp
            tests/gui/sfml/SelectionRectTests.cpp
            tests/gui/wx/PanelOptionsTests.cpp
            tests/gui/wx/WxGradientTests.cpp
            wxChaos/gui/sfml/SelectionRect.cpp
            wxChaos/gui/wx/PanelOptions.cpp
            wxChaos/gui/wx/wxGradient.cpp
    )

    target_include_directories(wxChaosGuiTests PRIVATE
            ${SFML_INCLUDE_DIRS}
            wxChaos/core
            wxChaos/gui
            wxChaos/gui/wx
    )

    target_link_libraries(wxChaosGuiTests PRIVATE
            doctest::doctest
            ${WXCHAOS_WX_LIBRARIES}
            sfml-graphics
            sfml-window
            sfml-system
    )

    target_compile_definitions(wxChaosGuiTests PRIVATE
            WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
            $<$<CONFIG:Debug>:__WXDEBUG__>
            _HAS_STD_BYTE=0
    )

    add_test(NAME wxChaosGuiTests COMMAND wxChaosGuiTests)
endif()
