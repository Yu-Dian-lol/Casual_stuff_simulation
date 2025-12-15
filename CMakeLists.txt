cmake_minimum_required(VERSION 3.10)

# 项目名称
project(Astar_WSL)

# 1. 设置 C++ 标准 (PGE 强制要求 C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 自动查找 Linux 下的图形依赖库
# WSL2 本质是 Linux，需要 X11 和 OpenGL 支持
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
# 通常也需要 PNG 库来加载图片
find_package(PNG REQUIRED) 

# 3. 设置头文件路径
# 这样你可以直接 #include "Astar.hpp" 和 #include "olcPixelGameEngine.h"

# 4. 设置源文件
set(SOURCE_FILES
    src/Astar.cpp
)

# 5. 生成可执行文件
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# ==========================================
# 5. 【后】设置头文件路径 (因为现在 target 已经存在了)
# ==========================================
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/olcPixelGameEngine
)
# 6. 链接库文件 (Linux/WSL 专用配置)
# 注意：pthread 和 stdc++fs 是必须的
target_link_libraries(${PROJECT_NAME} PRIVATE
    X11
    GL
    png
    pthread
    stdc++fs
)