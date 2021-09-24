cmake_minimum_required(VERSION 3.16)
project(TestAutomationFramework)

set(CMAKE_CXX_STANDARD 17)

#file(GLOB_RECURSE sources src/sink/**.cpp src/sink/**.h)

add_executable(test_automation_framework src/main.cpp src/pipe.h src/testframework.h src/testframework.cpp ${sources})
