cmake_minimum_required(VERSION 3.16)

project(redrose VERSION 0.6.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(FindPkgConfig)

message("Redrose version is ${PROJECT_VERSION}")
execute_process(COMMAND bash -c "(git describe --long --tags 2>/dev/null || echo stable) | tr -d '\n'" OUTPUT_VARIABLE PROJECT_REVISION)
set(PROJECT_TARGET ${PROJECT_NAME})
set(PROJECT_PREFIX ${CMAKE_INSTALL_PREFIX})
set(PROJECT_DATADIR ${CMAKE_INSTALL_PREFIX}/share)

option(ForceQt5 "ForceQt5" OFF)

add_subdirectory(app)
add_subdirectory(abc)
