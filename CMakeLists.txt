cmake_minimum_required(VERSION 3.28)
project(final_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(final_project src/main.cpp
        src/Rectangle.h
        src/DatabaseDef.h
        src/Bin.h
        src/Point.h
        src/Module.h
        src/Pin.h
        src/Net.h
        src/CellLibrary.h
        src/Database.h
        src/Row.h
        src/Database.cpp
        src/Placement.cpp
        src/Placement.h
)