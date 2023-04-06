cmake_minimum_required(VERSION 3.0)
#项目名称，不是最终生成的可执行文件
project(chat)  

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db) #构建项目时搜索server下的db目录
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)

# 指定搜索的子目录
add_subdirectory(src)  # 执行src文件夹下的CMakeLists.txt