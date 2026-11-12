cmake_minimum_required(VERSION 3.16)

# Fallback to in-tree vcpkg submodule if CMAKE_TOOLCHAIN_FILE is not set
if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "vcpkg toolchain")
endif()

# ── vcpkg configuration (Release-only overlay triplets) ──────────────────────
if(NOT DEFINED VCPKG_OVERLAY_TRIPLETS)
    set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/triplets" CACHE PATH "vcpkg overlay triplets")
endif()

project(MyWaveQt VERSION 1.0.0 LANGUAGES CXX)

# ── Compiler cache (ccache on Linux/macOS, sccache on Windows) ──────────────
find_program(CCACHE_PROGRAM ccache sccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   "${CCACHE_PROGRAM}" CACHE STRING "")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}" CACHE STRING "")
    message(STATUS "Using compiler cache: ${CCACHE_PROGRAM}")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_compile_definitions(
        _USE_MATH_DEFINES
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
endif()

if(MSVC)
    add_compile_options(
        /utf-8
        /permissive-
        /EHsc
    )
endif()

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 (system-installed)
find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Concurrent OpenGLWidgets REQUIRED)

# Find FFTW3 (via vcpkg)
find_package(FFTW3 CONFIG REQUIRED)
find_package(Threads REQUIRED)

# Find mimalloc (via vcpkg)
find_package(mimalloc CONFIG REQUIRED)

# Find OpenVINO (via vcpkg)
find_package(OpenVINO REQUIRED)

# ── Auto-discover source files ──────────────────────────────────────────────
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# ── Executable ───────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qcustomplot/qcustomplot.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qcustomplot/qcustomplot.h"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qcustomplot
)

# ── QCustomPlot GPU acceleration (compiled from source in 3rdparty/) ───────
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QCUSTOMPLOT_USE_OPENGL
    MYWAVE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/model"
)

# ── Assets synchronization ──────────────────────────────────────────────────
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Syncing assets to build directory"
)
add_dependencies(${PROJECT_NAME} copy_assets)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Syncing assets to target directory"
)

# Static Qt platform plugin (applied only when Qt was built as static library)
get_target_property(QT6_CORE_TYPE Qt6::Core TYPE)
if(QT6_CORE_TYPE STREQUAL "STATIC_LIBRARY")
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_STATICPLUGIN)
    if(LINUX AND TARGET Qt6::QWaylandIntegrationPlugin)
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QWaylandIntegrationPlugin)
    elseif(WIN32 AND TARGET Qt6::QWindowsIntegrationPlugin)
        target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QWindowsIntegrationPlugin)
    endif()
endif()

# ── Release optimization ─────────────────────────────────────────────────────
set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/arch:AVX2>
        $<$<CONFIG:Release>:/fp:fast>
        $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
    )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Concurrent
    Qt6::OpenGLWidgets
    FFTW3::fftw3
    Threads::Threads
    ${CMAKE_DL_LIBS}
    $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>
    openvino::runtime
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

# ── Windows deployment (windeployqt) ────────────────────────────────────────
if(WIN32)
    get_target_property(_qt_qmake_loc Qt6::qmake IMPORTED_LOCATION)
    if(_qt_qmake_loc)
        get_filename_component(_qt_bin_dir "${_qt_qmake_loc}" DIRECTORY)
    endif()
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_target(deploy_qt
            COMMAND ${WINDEPLOYQT_EXECUTABLE} --release --no-translations --compiler-runtime "$<TARGET_FILE:${PROJECT_NAME}>"
            DEPENDS ${PROJECT_NAME}
            COMMENT "Deploying Qt libraries with windeployqt"
        )
    endif()
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(MyWaveDspTests
        tests/DspParityTest.cpp
        src/core/Types.cpp
        src/core/DownSampler.cpp
        src/core/miniaudio_impl.cpp
        src/dsp/DspUtils.cpp
        src/dsp/EnergyCalculator.cpp
        src/dsp/FftProcessor.cpp
        src/dsp/FeatureExtractor.cpp
        src/dsp/LpcProcessor.cpp
        src/dsp/PitchDetector.cpp
        src/dsp/ZeroCrossingRate.cpp
        src/vad/ReferenceVad.cpp
        src/vad/SileroVad.cpp
    )
    target_include_directories(MyWaveDspTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_compile_definitions(MyWaveDspTests PRIVATE MYWAVE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/model")
    target_link_libraries(MyWaveDspTests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Concurrent
        FFTW3::fftw3
        openvino::runtime
        $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>
    )
    add_dependencies(MyWaveDspTests copy_assets)
    add_test(NAME dsp_reference_parity COMMAND MyWaveDspTests)

    add_executable(MyWaveUiTests
        tests/UiReactivityTest.cpp
        src/app/MainWindow.cpp
        src/core/AudioEngine.cpp
        src/core/AudioPlayer.cpp
        src/core/AudioDecoder.cpp
        src/core/AudioStorage.cpp
        src/core/ChartCache.cpp
        src/core/DownSampler.cpp
        src/core/Types.cpp
        src/core/miniaudio_impl.cpp
        src/dsp/DspUtils.cpp
        src/dsp/EnergyCalculator.cpp
        src/dsp/FeatureExtractor.cpp
        src/dsp/FftProcessor.cpp
        src/dsp/LpcProcessor.cpp
        src/dsp/PitchDetector.cpp
        src/dsp/ZeroCrossingRate.cpp
        src/vad/CepstralVad.cpp
        src/vad/EnergyVad.cpp
        src/vad/ReferenceVad.cpp
        src/vad/SileroVad.cpp
        src/vad/VadEngine.cpp
        src/vad/ZcrVad.cpp
        src/ui/ChartWidget.cpp
        src/ui/panels/ControlPanel.cpp
        src/ui/panels/InfoPanel.cpp
        src/ui/panels/HomePanel.cpp
        3rdparty/qcustomplot/qcustomplot.cpp
    )
    target_include_directories(MyWaveUiTests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qcustomplot
    )
    target_compile_definitions(MyWaveUiTests PRIVATE
        MYWAVE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/model"
        QCUSTOMPLOT_USE_OPENGL
    )
    target_link_libraries(MyWaveUiTests PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::PrintSupport
        Qt6::Concurrent
        Qt6::OpenGLWidgets
        FFTW3::fftw3
        openvino::runtime
        $<IF:$<TARGET_EXISTS:mimalloc-static>,mimalloc-static,mimalloc>
    )
    if(QT6_CORE_TYPE STREQUAL "STATIC_LIBRARY")
        target_compile_definitions(MyWaveUiTests PRIVATE QT_STATICPLUGIN)
        if(LINUX AND TARGET Qt6::QWaylandIntegrationPlugin)
            target_link_libraries(MyWaveUiTests PRIVATE Qt6::QWaylandIntegrationPlugin)
        elseif(WIN32 AND TARGET Qt6::QWindowsIntegrationPlugin)
            target_link_libraries(MyWaveUiTests PRIVATE Qt6::QWindowsIntegrationPlugin)
        endif()
    endif()
    add_dependencies(MyWaveUiTests copy_assets)
    add_test(NAME ui_reactivity COMMAND MyWaveUiTests)
endif()
