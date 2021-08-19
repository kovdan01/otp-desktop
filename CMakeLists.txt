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

# OATH toolkit https://www.nongnu.org/oath-toolkit/ - OTP generation
pkg_check_modules(oath REQUIRED IMPORTED_TARGET liboath)

# Libcrypto - encrypt secrets file
#pkg_check_modules(crypto REQUIRED IMPORTED_TARGET libcrypto)

# yaml-cpp - work with configuration file
find_package(yaml-cpp REQUIRED)

# Qt5 - UI
find_package(
    Qt5 REQUIRED
    COMPONENTS Widgets LinguistTools Sql
)

# ZXing https://github.com/nu-book/zxing-cpp - QR code scanning
#find_package(ZXing REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/ui)

set(HEADERS
    include/add_item_dialog.hpp
    include/crypto.hpp
    include/main_window.hpp
    include/otp_list.hpp
    include/shit.hpp
    include/totp.hpp
)

set(SOURCES
    src/add_item_dialog.cpp
    src/crypto.cpp
    src/main.cpp
    src/main_window.cpp
    src/otp_list.cpp
    src/shit.cpp
    src/totp.cpp
)

set(UI_FILES
    ui/add_item_dialog.ui
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
#target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::crypto)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)
#target_link_libraries(${PROJECT_NAME} PRIVATE ZXing::ZXing)

ntc_target(${PROJECT_NAME}
#    TRANSLATIONS en_US
)

include(FeatureSummary)
feature_summary(WHAT ALL)
