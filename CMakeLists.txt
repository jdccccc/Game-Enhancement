# 项目基础配置
cmake_minimum_required(VERSION 3.10)
project(RanderLearning CXX C)

# 启用C语言支持
enable_language(C)

# 设置C++标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 创建可执行文件
# 将 main.cpp 和 glad.c 一起编译成一个名为 "main" 的可执行文件
# 这是最关键的一步，必须包含 glad.c
add_executable(main main.cpp glad.c)

# 添加头文件搜索路径
# 告诉编译器去 "include" 文件夹里寻找 #include <glad/glad.h>
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 查找并链接外部库
# 查找系统上安装的 GLFW3 库
find_package(glfw3 REQUIRED)

# 设置策略 CMP0072 为 NEW，明确告知 FindOpenGL 优先使用 GLVND
cmake_policy(SET CMP0072 NEW)
# 查找系统上的 OpenGL 库
find_package(OpenGL REQUIRED)

# 查找线程库 (GLFW 常常需要)
find_package(Threads REQUIRED)

# 将找到的库链接到我们的可执行文件上
target_link_libraries(main PRIVATE glfw ${OPENGL_LIBRARIES} Threads::Threads)

# (可选, 推荐) 设置输出目录，让可执行文件生成在项目根目录的 "bin" 文件夹下
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)