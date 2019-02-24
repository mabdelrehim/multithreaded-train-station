cmake_minimum_required(VERSION 3.13)
project(train_station C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")

add_executable(train_station main.c train.c train.h)