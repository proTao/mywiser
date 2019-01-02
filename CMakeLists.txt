# CMAKE 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (mywyser)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")


# 指定生成目标
add_executable(main main.cpp indexer.cpp)
