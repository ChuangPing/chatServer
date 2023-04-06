# 定义了一个SRC_LIST变量，包含了该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST) #声明变量：当前目录（src下）db目录下的所有文件
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./redis REDIS_LIST)

# 指定生成可执行文件
add_executable(chatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})

# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(chatServer muduo_net muduo_base  mysqlclient hiredis pthread)  # 指定MySQL开发库 hiredis