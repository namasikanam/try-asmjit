cmake_minimum_required(VERSION 3.0)

# 设置构建类型为Debug
set(CMAKE_BUILD_TYPE Debug)
set(ASMJIT_BUILD_DEBUG)

# 添加调试信息选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

# 提供一个选项来启用 AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
  # 对于 Clang，也可以考虑添加 -fno-omit-frame-pointer 来获得更好的堆栈跟踪
endif()

# 添加 AsmJit 子目录
add_subdirectory(./asmjit)

# 创建可执行文件
add_executable(main main.cpp)

# 链接 AsmJit 库到你的目标
target_link_libraries(main PRIVATE asmjit)
