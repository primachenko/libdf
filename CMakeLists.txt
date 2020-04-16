cmake_minimum_required(VERSION 2.8.11)
project(df)

add_definitions(-Wall)
include_directories(include)
set(SRC
    src/df.c)
add_library(${PROJECT_NAME} STATIC ${SRC})