cmake_minimum_required(VERSION 3.14)

project(mcrpc-qt-cli LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)
find_package(QuaZip-Qt6)
# find_package(zlib)
include_directories(${QUAZIP_INCLUDE_DIRS})

add_executable(mcrpc-qt-cli
  main.cpp
  mcrpc.h
  mcrpc.cpp
)
target_link_libraries(mcrpc-qt-cli Qt${QT_VERSION_MAJOR}::Core QuaZip::QuaZip Qt${QT_VERSION_MAJOR}::Network)
