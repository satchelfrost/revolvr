cmake_minimum_required(VERSION 3.10.2)
project(parser)

file(GLOB LOCAL_SOURCE "*.cpp")

add_executable(parser ${LOCAL_SOURCE})