#========================================================================================
# CMake configuration for wxChaos
#========================================================================================
cmake_minimum_required(VERSION 3.15)
project(wxChaos LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/win32_$<$<CONFIG:Debug>:wx3 Unicode Debug>$<$<CONFIG:Release>:wx3 Unicode Release>)

#========================================================================================
# 1. FIND DEPENDENCIES FIRST
#========================================================================================

# --- SFML Configuration (Classic Variable-based approach) ---
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "${CMAKE_SOURCE_DIR}/libs/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6.2 COMPONENTS system window graphics main REQUIRED)

# --- wxWidgets Configuration ---
set(WXWIDGETS_ROOT_DIR "${CMAKE_SOURCE_DIR}/libs/wxMSW-3.1.4")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WXWIDGETS_ARCH_DIR vc_x64_lib)
    message(STATUS "Configuring for 64-bit build (x64).")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(WXWIDGETS_ARCH_DIR vc_lib)
    message(STATUS "Configuring for 32-bit build (Win32).")
else()
    message(FATAL_ERROR "Unsupported architecture.")
endif()
set(WXWIDGETS_LIB_DIR "${WXWIDGETS_ROOT_DIR}/lib/${WXWIDGETS_ARCH_DIR}")

set(WXWIDGETS_LIBRARIES_RELEASE
        wxmsw31u_core wxbase31u wxbase31u_xml wxmsw31u_adv wxmsw31u_richtext
        wxmsw31u_html wxmsw31u_stc wxscintilla wxtiff wxjpeg wxpng wxzlib
        wxregexu wxexpat
)
set(WXWIDGETS_LIBRARIES_DEBUG
        wxmsw31ud_core wxbase31ud wxbase31ud_xml wxmsw31ud_adv wxmsw31ud_richtext
        wxmsw31ud_html wxmsw31ud_stc wxscintillad wxtiffd wxjpegd wxpngd
        wxzlibd wxregexud wxexpatd
)

#========================================================================================
# 2. DEFINE THE TARGET
#========================================================================================
set(PROJECT_SOURCES
        wxChaos/About.cpp
        wxChaos/AngelscriptEngine.cpp
        wxChaos/BinaryOps.cpp
        wxChaos/BmpWriter.cpp
        wxChaos/ColorFrame.cpp
        wxChaos/ConfigParser.cpp
        wxChaos/ConstDialog.cpp
        wxChaos/DimensionFrame.cpp
        wxChaos/Filesystem.cpp
        wxChaos/FormulaDialog.cpp
        wxChaos/FractalCanvas.cpp
        wxChaos/FractalClasses.cpp
        wxChaos/FractalGUI.cpp
        wxChaos/FractalTypes.cpp
        wxChaos/gradient.cpp
        wxChaos/gradientdlg.cpp
        wxChaos/HTMLViewer.cpp
        wxChaos/IterDialog.cpp
        wxChaos/JuliaMode.cpp
        wxChaos/MainWindow.cpp
        wxChaos/ScriptEditor.cpp
        wxChaos/SizeDialogSave.cpp
        wxChaos/StringFuncs.cpp
        wxChaos/wxSFMLCanvas.cpp
        wxChaos/ZoomRecorder.cpp
        wxChaos/angelscript/add_on/scriptarray/scriptarray.cpp
        wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring.cpp
        wxChaos/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp
        wxChaos/angelscript/src/as_atomic.cpp
        wxChaos/angelscript/src/as_builder.cpp
        wxChaos/angelscript/src/as_bytecode.cpp
        wxChaos/angelscript/src/as_callfunc.cpp
        wxChaos/angelscript/src/as_callfunc_x64_msvc.cpp
        wxChaos/angelscript/src/as_callfunc_x86.cpp
        wxChaos/angelscript/src/as_compiler.cpp
        wxChaos/angelscript/src/as_configgroup.cpp
        wxChaos/angelscript/src/as_context.cpp
        wxChaos/angelscript/src/as_datatype.cpp
        wxChaos/angelscript/src/as_gc.cpp
        wxChaos/angelscript/src/as_generic.cpp
        wxChaos/angelscript/src/as_globalproperty.cpp
        wxChaos/angelscript/src/as_memory.cpp
        wxChaos/angelscript/src/as_module.cpp
        wxChaos/angelscript/src/as_objecttype.cpp
        wxChaos/angelscript/src/as_outputbuffer.cpp
        wxChaos/angelscript/src/as_parser.cpp
        wxChaos/angelscript/src/as_restore.cpp
        wxChaos/angelscript/src/as_scriptcode.cpp
        wxChaos/angelscript/src/as_scriptengine.cpp
        wxChaos/angelscript/src/as_scriptfunction.cpp
        wxChaos/angelscript/src/as_scriptnode.cpp
        wxChaos/angelscript/src/as_scriptobject.cpp
        wxChaos/angelscript/src/as_string.cpp
        wxChaos/angelscript/src/as_string_util.cpp
        wxChaos/angelscript/src/as_thread.cpp
        wxChaos/angelscript/src/as_tokenizer.cpp
        wxChaos/angelscript/src/as_typeinfo.cpp
        wxChaos/angelscript/src/as_variablescope.cpp
        wxChaos/muParserX/mpError.cpp
        wxChaos/muParserX/mpFuncCmplx.cpp
        wxChaos/muParserX/mpFuncCommon.cpp
        wxChaos/muParserX/mpFuncMatrix.cpp
        wxChaos/muParserX/mpFuncNonCmplx.cpp
        wxChaos/muParserX/mpFuncStr.cpp
        wxChaos/muParserX/mpICallback.cpp
        wxChaos/muParserX/mpIfThenElse.cpp
        wxChaos/muParserX/mpIOprt.cpp
        wxChaos/muParserX/mpIPackage.cpp
        wxChaos/muParserX/mpIToken.cpp
        wxChaos/muParserX/mpIValReader.cpp
        wxChaos/muParserX/mpIValue.cpp
        wxChaos/muParserX/mpOprtBinAssign.cpp
        wxChaos/muParserX/mpOprtBinCommon.cpp
        wxChaos/muParserX/mpOprtCmplx.cpp
        wxChaos/muParserX/mpOprtIndex.cpp
        wxChaos/muParserX/mpOprtMatrix.cpp
        wxChaos/muParserX/mpOprtNonCmplx.cpp
        wxChaos/muParserX/mpPackageCmplx.cpp
        wxChaos/muParserX/mpPackageCommon.cpp
        wxChaos/muParserX/mpPackageMatrix.cpp
        wxChaos/muParserX/mpPackageNonCmplx.cpp
        wxChaos/muParserX/mpPackageStr.cpp
        wxChaos/muParserX/mpPackageUnit.cpp
        wxChaos/muParserX/mpParser.cpp
        wxChaos/muParserX/mpParserBase.cpp
        wxChaos/muParserX/mpRPN.cpp
        wxChaos/muParserX/mpScriptTokens.cpp
        wxChaos/muParserX/mpTest.cpp
        wxChaos/muParserX/mpTokenReader.cpp
        wxChaos/muParserX/mpValReader.cpp
        wxChaos/muParserX/mpValue.cpp
        wxChaos/muParserX/mpValueCache.cpp
        wxChaos/muParserX/mpVariable.cpp
        wxChaos/wxMathPlot/mathplot.cpp
)

add_executable(wxChaos WIN32 ${PROJECT_SOURCES})

#========================================================================================
# 3. CONFIGURE TARGET PROPERTIES (INCLUDES, DEFINITIONS, LIBRARIES)
#========================================================================================

# --- Include Directories ---
target_include_directories(wxChaos PRIVATE
        "${WXWIDGETS_ROOT_DIR}/include"
        "${WXWIDGETS_ROOT_DIR}/include/msvc"
        "${SFML_INCLUDE_DIRS}" # <--- THIS IS THE FIX. Note the "S" at the end.
)

# --- Compile Definitions ---
target_compile_definitions(wxChaos PRIVATE
        WIN32 _WINDOWS __WXMSW__ _CRT_SECURE_NO_WARNINGS
        "SFML_STATIC"
        UNICODE _UNICODE
        $<$<CONFIG:Debug>:__WXDEBUG__>
)

# --- Link Libraries and Directories ---
target_link_directories(wxChaos PRIVATE "${WXWIDGETS_LIB_DIR}")

target_link_libraries(wxChaos PRIVATE
        ${SFML_LIBRARIES}

        # wxWidgets Libraries
        $<$<CONFIG:Release>:${WXWIDGETS_LIBRARIES_RELEASE}>
        $<$<CONFIG:Debug>:${WXWIDGETS_LIBRARIES_DEBUG}>

        # Windows System Libraries
        winmm comctl32 rpcrt4 wsock32 opengl32 freetype
)

# --- MSVC Specific Settings ---
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    target_link_options(wxChaos PRIVATE "/NODEFAULTLIB:libcmt.lib")
    target_compile_options(wxChaos PRIVATE
            $<$<CONFIG:Release>:/fp:fast>
            $<$<CONFIG:Release>:/Ot>
    )
    set_property(TARGET wxChaos PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif()

#========================================================================================
# 4. POST-BUILD STEPS
#========================================================================================
add_custom_command(
        TARGET wxChaos POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/app_resources/English"
        "$<TARGET_FILE_DIR:wxChaos>/English"
        COMMENT "Copying application resources..."
)