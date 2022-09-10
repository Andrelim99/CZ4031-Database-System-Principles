cmake_minimum_required(VERSION 3.23)
project(CZ4031_Proj1)

set(CMAKE_CXX_STANDARD 14)

add_executable(CZ4031_Proj1 Project1/main.cpp Project1/BPlusTree.cpp Project1/BPlusTree.h Project1/MemoryPool.cpp Project1/MemoryPool.h)
