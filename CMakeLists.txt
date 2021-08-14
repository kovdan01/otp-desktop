cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Interprocedural optimization
cmake_policy(SET CMP0069 NEW)

project(otp-desktop
    VERSION 0.0.0
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/ntc-cmake/ntc-cmake")

include(ntc-dev-build)

set(THREADS_PREFER_PTHERAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(oath REQUIRED IMPORTED_TARGET liboath)

find_package(
    Qt5 REQUIRED
    COMPONENTS Widgets LinguistTools Sql
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui)

set(HEADERS
    include/main_window.hpp
)

set(SOURCES
    src/main.cpp
    src/main_window.cpp
)

set(UI_FILES
    ui/main_window.ui
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${UI_FILES}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::oath)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

ntc_target(${PROJECT_NAME}
#    TRANSLATIONS en_US
)

include(FeatureSummary)
feature_summary(WHAT ALL)
