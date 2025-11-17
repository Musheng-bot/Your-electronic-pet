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

add_executable(Pet main.cpp)
target_link_libraries(Pet
		Qt::Core
		Qt::Gui
		Qt::Widgets
)

