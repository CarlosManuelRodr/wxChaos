file(GLOB_RECURSE WXCHAOS_SRC CONFIGURE_DEPENDS
    *.cpp
)

add_executable(wxChaosApp ${WXCHAOS_SRC})

# Include directories
target_include_directories(wxChaosApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Detect the wxWidgets configuration directory that contains wx/setup.h
file(GLOB WX_SETUP_DIR RELATIVE
    "${wxwidgets_BINARY_DIR}/lib/wx/include" "${wxwidgets_BINARY_DIR}/lib/wx/include/*")

target_include_directories(wxChaosApp PRIVATE
    "${wxwidgets_SOURCE_DIR}/include"
    "${wxwidgets_BINARY_DIR}/lib/wx/include/${WX_SETUP_DIR}"
    "${SFML_SOURCE_DIR}/include"
)

# Link libraries
target_link_libraries(wxChaosApp PRIVATE
    sfml-graphics sfml-window sfml-system
    wxwidgets
)

# On Windows link libs for wxWidgets
if (WIN32)
    target_link_libraries(wxChaosApp PRIVATE winmm imm32 version uxtheme)
endif()
