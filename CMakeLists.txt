cmake_minimum_required(VERSION 3.16)

project(InputSwitcher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理 Qt 的 MOC、UIC、RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt6 包
find_package(Qt6 REQUIRED COMPONENTS Core)

# 源文件
set(PROJECT_SOURCES
    main.cpp
    inputswitcher.cpp
    inputswitcher.h
)

# 创建可执行文件
# WIN32 标志会创建 Windows GUI 应用（不显示控制台）
add_executable(InputSwitcher WIN32 ${PROJECT_SOURCES})

# 链接 Qt 库
target_link_libraries(InputSwitcher PRIVATE Qt6::Core)

# Windows 特定库
if(WIN32)
    target_link_libraries(InputSwitcher PRIVATE Psapi User32)
endif()

# 设置输出目录
set_target_properties(InputSwitcher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows 子系统设置（确保不显示控制台）
if(MSVC)
    set_target_properties(InputSwitcher PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
elseif(MINGW)
    set_target_properties(InputSwitcher PROPERTIES
        LINK_FLAGS "-mwindows"
    )
endif()
