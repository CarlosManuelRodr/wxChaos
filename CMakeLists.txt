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
find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base adv html stc richtext webview xml)
set(WXCHAOS_WX_LIBRARIES
        wx::core
        wx::base
        wx::adv
        wx::html
        wx::stc
        wx::richtext
        wx::webview
        wx::xml
)

find_package(doctest CONFIG REQUIRED)
if(WIN32)
        set(WXCHAOS_SFML_COMPONENTS graphics window system main)
else()
        set(WXCHAOS_SFML_COMPONENTS graphics window system)
endif()
find_package(SFML 2.6.2 CONFIG REQUIRED COMPONENTS ${WXCHAOS_SFML_COMPONENTS})
find_package(Angelscript CONFIG REQUIRED)
find_package(Gettext)

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

if(UNIX AND NOT APPLE)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GSTREAMER REQUIRED IMPORTED_TARGET gstreamer-1.0 gstreamer-app-1.0)
endif()

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
        wxChaos/gui/wx
        wxChaos/utils
        wxChaos/scripting
        wxChaos/scripting/angelscript
        third_party/muParserX
        third_party/wxMathPlot
)

# -------------------------------------------------------------------
# Definitions
# -------------------------------------------------------------------
target_compile_definitions(wxChaos PRIVATE
        WXCHAOS_BUILD_TYPE="$<CONFIG>"
        $<$<CONFIG:Debug>:__WXDEBUG__>
)

if(WIN32)
        target_compile_definitions(wxChaos PRIVATE WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_)
endif()

if(MSVC)
        target_compile_definitions(wxChaos PRIVATE _HAS_STD_BYTE=0) # turns off the enum class std::byte in MSVC’s STL
endif()

# -------------------------------------------------------------------
# Linking
# -------------------------------------------------------------------
target_link_libraries(wxChaos PRIVATE
        ${WXCHAOS_WX_LIBRARIES}
        sfml-graphics
        sfml-window
        sfml-system
        Angelscript::angelscript
        ${SYMENGINE_LIBRARIES}
        ${MPFR_LIBRARY}
        ${GMP_LIBRARY}
        Threads::Threads          # pthreads
)

if(GETTEXT_FOUND)
        file(GLOB WXCHAOS_PO_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/locale/*/wxchaos.po")
        foreach(WXCHAOS_PO_FILE ${WXCHAOS_PO_FILES})
                get_filename_component(WXCHAOS_LOCALE_DIR "${WXCHAOS_PO_FILE}" DIRECTORY)
                get_filename_component(WXCHAOS_LOCALE "${WXCHAOS_LOCALE_DIR}" NAME)
                set(WXCHAOS_MO_FILE
                        "${CMAKE_SOURCE_DIR}/app_resources/Resources/Locale/${WXCHAOS_LOCALE}/LC_MESSAGES/wxchaos.mo")
                add_custom_command(
                        OUTPUT "${WXCHAOS_MO_FILE}"
                        COMMAND ${CMAKE_COMMAND} -E make_directory
                        "${CMAKE_SOURCE_DIR}/app_resources/Resources/Locale/${WXCHAOS_LOCALE}/LC_MESSAGES"
                        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o "${WXCHAOS_MO_FILE}" "${WXCHAOS_PO_FILE}"
                        DEPENDS "${WXCHAOS_PO_FILE}"
                        COMMENT "Compiling ${WXCHAOS_LOCALE} wxChaos translation catalog..."
                )
                list(APPEND WXCHAOS_MO_FILES "${WXCHAOS_MO_FILE}")
        endforeach()

        if(WXCHAOS_MO_FILES)
                add_custom_target(wxChaosTranslations DEPENDS ${WXCHAOS_MO_FILES})
                add_dependencies(wxChaos wxChaosTranslations)
        endif()
endif()

if(WIN32)
        target_link_libraries(wxChaos PRIVATE
                sfml-main
                winmm comctl32 rpcrt4 wsock32 opengl32
                mfplat mfreadwrite mfuuid ole32
        )
elseif(UNIX AND NOT APPLE)
        target_link_libraries(wxChaos PRIVATE PkgConfig::GSTREAMER)
endif()

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
    file(GLOB_RECURSE WXCHAOS_CORE_TEST_SOURCES CONFIGURE_DEPENDS
            wxChaos/core/*.cpp
            wxChaos/scripting/*.cpp
            wxChaos/utils/*.cpp
            third_party/muParserX/*.cpp
    )

    add_executable(wxChaosCoreTests
            tests/core/CoreTestsMain.cpp
            tests/core/FractalUtilitiesTests.cpp
            tests/core/FractalInspectionTests.cpp
            tests/core/RenderingAlgorithmTests.cpp
            tests/core/geometry/GeometryTests.cpp
            tests/core/rendering/RenderingTests.cpp
            tests/core/rendering/HenonMapRendererTests.cpp
            tests/core/rendering/LogisticMapRendererTests.cpp
            tests/core/UserFormulaSymbolicCompilerTests.cpp
            tests/core/NewtonRootRegistryTests.cpp
            ${WXCHAOS_CORE_TEST_SOURCES}
            wxChaos/gui/wx/common/PanelOptions.cpp
            wxChaos/gui/wx/common/wxGradient.cpp
            wxChaos/gui/wx/docs/FractalDocumentation.cpp
    )

    target_include_directories(wxChaosCoreTests PRIVATE
            ${SYMENGINE_INCLUDE_DIRS}
            ${SFML_INCLUDE_DIRS}
            wxChaos/core
            wxChaos/gui
            wxChaos/gui/wx
            wxChaos/utils
            wxChaos/scripting
            wxChaos/scripting/angelscript
            third_party/muParserX
    )

    target_link_libraries(wxChaosCoreTests PRIVATE
            doctest::doctest
            ${WXCHAOS_WX_LIBRARIES}
            Angelscript::angelscript
            ${SYMENGINE_LIBRARIES}
            ${MPFR_LIBRARY}
            ${GMP_LIBRARY}
            sfml-graphics
            sfml-window
            sfml-system
            Threads::Threads
    )

    if(MSVC)
        target_compile_definitions(wxChaosCoreTests PRIVATE
                WXCHAOS_BUILD_TYPE="$<CONFIG>"
                WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_
                $<$<CONFIG:Debug>:__WXDEBUG__>
                _HAS_STD_BYTE=0
        )
    endif()

    add_test(NAME wxChaosCoreTests COMMAND wxChaosCoreTests)

    add_executable(wxChaosGuiTests
            tests/gui/GuiTestsMain.cpp
            tests/gui/sfml/SelectionRectTests.cpp
            tests/gui/wx/DocumentationLinkActionTests.cpp
            tests/gui/wx/FractalDocumentationTests.cpp
            tests/gui/wx/AppConfigStoreTests.cpp
            tests/gui/wx/AppLocalizationTests.cpp
            tests/gui/wx/PanelOptionsTests.cpp
            tests/gui/wx/WxGradientTests.cpp
            wxChaos/core/coloring/ColorPalette.cpp
            wxChaos/gui/sfml/SelectionRect.cpp
            wxChaos/gui/wx/config/AppConfigStore.cpp
            wxChaos/gui/wx/common/AppTheme.cpp
            wxChaos/gui/wx/common/AppLocalization.cpp
            wxChaos/gui/wx/docs/DocumentationLinkAction.cpp
            wxChaos/gui/wx/docs/FractalDocumentation.cpp
            wxChaos/gui/wx/common/PanelOptions.cpp
            wxChaos/gui/wx/common/wxGradient.cpp
            wxChaos/scripting/ScriptData.cpp
            wxChaos/utils/AppPaths.cpp
    )

    target_include_directories(wxChaosGuiTests PRIVATE
            ${SFML_INCLUDE_DIRS}
            wxChaos/core
            wxChaos/gui
            wxChaos/gui/wx
            wxChaos/scripting
            wxChaos/utils
    )

    target_link_libraries(wxChaosGuiTests PRIVATE
            doctest::doctest
            ${WXCHAOS_WX_LIBRARIES}
            sfml-graphics
            sfml-window
            sfml-system
    )

    target_compile_definitions(wxChaosGuiTests PRIVATE $<$<CONFIG:Debug>:__WXDEBUG__>)

    if(WIN32)
        target_compile_definitions(wxChaosGuiTests PRIVATE WIN32 _WINDOWS UNICODE _UNICODE WIN32_LEAN_AND_MEAN _WINSOCKAPI_)
    endif()

    if(MSVC)
        target_compile_definitions(wxChaosGuiTests PRIVATE _HAS_STD_BYTE=0)
    endif()

    add_test(NAME wxChaosGuiTests COMMAND wxChaosGuiTests)
endif()
