cmake_minimum_required(VERSION 3.16)

project(redrose VERSION 0.5.19 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(FindPkgConfig)

message("Redrose version is ${PROJECT_VERSION}")
execute_process(COMMAND sh -c "git status > /dev/null 2>&1 && git describe --long --tags | tr -d '\n' || echo -n stable" OUTPUT_VARIABLE PROJECT_REVISION)
set(PROJECT_TARGET ${PROJECT_NAME})
set(PROJECT_PREFIX ${CMAKE_INSTALL_PREFIX})
set(PROJECT_DATADIR ${CMAKE_INSTALL_PREFIX}/share)

add_subdirectory(app)
add_subdirectory(abc)
