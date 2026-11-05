cmake_minimum_required(VERSION 3.15)
project(StudentManagementSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(sms
    src/main.cpp
    src/Student.cpp
    src/StudentManager.cpp
    src/Utils.cpp
)

add_executable(sms_single
    main_single.cpp
)
