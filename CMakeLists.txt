cmake_minimum_required (VERSION 3.17)

project (BluetoothApp VERSION 0.1 LANGUAGES CXX)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

set (CMAKE_AUTOMOC ON)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Weffc++ -Wextra")

find_package (Qt6 REQUIRED COMPONENTS Widgets Bluetooth)

set (PROJECT_SOURCES
        Bluetooth.cc        Bluetooth.hh
        main.cc
)

qt_add_executable (bluetooth MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries (bluetooth PRIVATE Qt::Widgets Qt::Bluetooth)

qt_finalize_executable (bluetooth)
