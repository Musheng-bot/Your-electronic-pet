cmake_minimum_required(VERSION 4.0)
project(Pet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/musheng/Qt/6.9.1/gcc_64")

find_package(Qt6 COMPONENTS
		Core
		Gui
		Widgets
		REQUIRED)

qt_add_resources(RESOURCES resources.qrc)

add_executable(Pet main.cpp
		Pet.cpp
		Pet.hpp ${RESOURCES})
target_link_libraries(Pet
		Qt::Core
		Qt::Gui
		Qt::Widgets
)
target_compile_definitions(Pet PRIVATE QT_NO_DEBUG_OUTPUT)

