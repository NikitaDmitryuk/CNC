cmake_minimum_required(VERSION 3.15)
project(CNC)

set(MYCOMPILE_FLAGS "-pthread")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${MYCOMPILE_FLAGS}")

add_executable(CNC main.cpp Engine.cpp Engine.h LeftEngine.cpp LeftEngine.h RightEngine.cpp RightEngine.h Printer.cpp Printer.h)