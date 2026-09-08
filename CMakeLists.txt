cmake_minimum_required(VERSION 3.16)
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

# Enable Qt features
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 (via vcpkg)
find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Multimedia Concurrent OpenGLWidgets REQUIRED)

# Find FFTW3 (via vcpkg)
find_package(FFTW3 CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)

# ── Auto-discover source files (includes 3rdparty/qcustomplot.cpp) ──────────
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# ── Executable ───────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/3rdparty
)

# ── QCustomPlot GPU acceleration (compiled from source in 3rdparty/) ───────
target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_USE_OPENGL)

# Static Qt platform plugin
target_compile_definitions(${PROJECT_NAME} PRIVATE QT_STATICPLUGIN)
if(LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QWaylandIntegrationPlugin)
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::QWindowsIntegrationPlugin)
endif()

# ── Release optimization ─────────────────────────────────────────────────────
set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/arch:AVX2>
        $<$<CONFIG:Release>:/fp:fast>
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
    Qt6::Multimedia
    Qt6::Concurrent
    Qt6::OpenGLWidgets
    FFTW3::fftw3
    SndFile::sndfile
)

include(CTest)
if(BUILD_TESTING)
    add_executable(MyWaveDspTests
        tests/DspParityTest.cpp
        src/core/Types.cpp
        src/core/DownSampler.cpp
        src/dsp/DspUtils.cpp
        src/dsp/FftProcessor.cpp
        src/dsp/FeatureExtractor.cpp
        src/dsp/LpcProcessor.cpp
        src/dsp/PitchDetector.cpp
        src/vad/ReferenceVad.cpp
    )
    target_include_directories(MyWaveDspTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(MyWaveDspTests PRIVATE Qt6::Core Qt6::Gui Qt6::Concurrent FFTW3::fftw3)
    add_test(NAME dsp_reference_parity COMMAND MyWaveDspTests)
endif()
