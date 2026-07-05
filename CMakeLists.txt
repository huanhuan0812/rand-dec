cmake_minimum_required(VERSION 3.16)

project(rand-dec VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============ 动态/静态Qt配置 ============
# 默认使用动态Qt，用户可通过 -DQT_STATIC_BUILD=ON 启用静态链接
set(QT_STATIC_BUILD OFF CACHE BOOL "使用静态Qt链接（需要静态Qt库）")

# 提示用户设置Qt路径
if(NOT Qt6_DIR)
    message(STATUS "提示: 如需指定Qt路径，请设置 Qt6_DIR")
    message(STATUS "示例: -DQt6_DIR=C:/Qt/6.5.0/msvc2019_64/lib/cmake/Qt6")
endif()

if(QT_STATIC_BUILD)
    message(STATUS "⚠️  启用静态Qt链接模式")
    message(STATUS "确保 Qt6_DIR 指向静态Qt的lib/cmake/Qt6 目录")
    # 静态Qt需要将BUILD_SHARED_LIBS设为OFF
    set(BUILD_SHARED_LIBS OFF)
else()
    message(STATUS "使用动态Qt链接（默认）")
    set(BUILD_SHARED_LIBS ON)
endif()

# ============ 平台检测 ============
if(WIN32)
    # Windows特定设置
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # 设置Windows SDK版本为Win10 1803 (10.0.17134.0)
        set(CMAKE_SYSTEM_VERSION "10.0.17134.0" CACHE STRING "Windows SDK version" FORCE)
        
        # ARM64 架构检测
        if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64" OR CMAKE_GENERATOR_PLATFORM STREQUAL "arm64")
            set(ARCH_ARM64 TRUE)
            message(STATUS "Building for ARM64 architecture")
        else()
            set(ARCH_X64 TRUE)
            message(STATUS "Building for x64 architecture")
        endif()
        
        # 只在静态链接模式下使用 /MT
        if(QT_STATIC_BUILD)
            # 静态链接MSVC运行时库 (/MT)
            foreach(flag_var 
                CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
                CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
                if(${flag_var} MATCHES "/MD")
                    string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
                endif()
                if(${flag_var} MATCHES "/MDd")
                    string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
                endif()
            endforeach()
            message(STATUS "MSVC运行时: 静态链接 (/MT)")
        else()
            message(STATUS "MSVC运行时: 动态链接 (/MD) [默认]")
        endif()
        
        # ============ 优化和死代码消除 ============
        # 编译器优化（Release模式）
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gy")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:inline")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GS-")
        
        # 链接器优化 - 死代码消除
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:ICF")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO")
        
        # 生成调试信息（PDB）
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    endif()
endif()

# ============ 查找Qt ============
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 验证Qt类型
if(TARGET Qt6::Core)
    get_target_property(QT_TYPE Qt6::Core TYPE)
    if(QT_STATIC_BUILD)
        if(NOT QT_TYPE STREQUAL "STATIC_LIBRARY")
            message(WARNING "⚠️  请求静态Qt但找到的是动态Qt！请确保 Qt6_DIR 指向静态Qt的路径")
            message(WARNING "当前Qt类型: ${QT_TYPE}")
        else()
            message(STATUS "✓ 使用静态Qt")
        endif()
    else()
        if(QT_TYPE STREQUAL "STATIC_LIBRARY")
            message(WARNING "⚠️  使用动态模式但找到静态Qt！如需静态链接请设置 -DQT_STATIC_BUILD=ON")
        else()
            message(STATUS "✓ 使用动态Qt")
        endif()
    endif()
endif()

qt_standard_project_setup()

# ============ Qt特定优化 ============
if(QT_STATIC_BUILD AND MSVC)
    # 禁用Qt调试输出（Release模式下）
    add_definitions(-DQT_NO_DEBUG)
    add_definitions(-DQT_STRICT_ITERATORS)
    add_definitions(-DQT_MESSAGELOGCONTEXT)
    
    # 可选：禁用不需要的Qt功能来减小体积
    # add_definitions(-DQT_NO_PRINTER)
    # add_definitions(-DQT_NO_PRINTDIALOG)
    # add_definitions(-DQT_NO_OPENGL)
    # add_definitions(-DQT_NO_SQL)
    # add_definitions(-DQT_NO_XML)
endif()

# ============ 资源文件 ============
if(WIN32)
    set(APP_ICON_RESOURCES "resources.rc")
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
endif()

# ============ 可执行文件 ============
qt_add_executable(rand-dec
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${APP_ICON_RESOURCES}
)

# ============ Qt模块链接 ============
target_link_libraries(rand-dec PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)

# ============ Qt静态插件 ============
# 仅在静态链接时包含插件
if(QT_STATIC_BUILD)
    qt_import_plugins(rand-dec 
        INCLUDE 
            Qt6::QWindowsIntegrationPlugin
        EXCLUDE 
            Qt6::QMinimalIntegrationPlugin
            Qt6::QOffscreenIntegrationPlugin
    )
endif()

# ============ 平台特定链接 ============
if(WIN32)
    # Windows系统库
    target_link_libraries(rand-dec PRIVATE
        user32
        gdi32
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
        ws2_32
        d3d11
        dxgi
        dwmapi
        uxtheme
        shlwapi
        imm32
        winmm
        msimg32
    )
    
    # ARM64特定链接库
    if(ARCH_ARM64)
        target_link_libraries(rand-dec PRIVATE
            # ARM64特定库（如有需要）
        )
    endif()
    
    # Windows应用程序设置
    set_target_properties(rand-dec PROPERTIES
        WIN32_EXECUTABLE TRUE
        RC_ICONS "app.ico"
    )
endif()

# ============ macOS 捆绑包设置 ============
if(APPLE)
    set_target_properties(rand-dec PROPERTIES
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
    )
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app.icns")
        set_source_files_properties(app.icns PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(rand-dec PRIVATE app.icns)
        set_target_properties(rand-dec PROPERTIES
            MACOSX_BUNDLE_ICON_FILE app.icns)
    endif()
endif()

# ============ Linux 设置 ============
if(LINUX)
    # Linux特定设置（如需要）
endif()

# ============ 安装 ============
include(GNUInstallDirs)
install(TARGETS rand-dec
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ============ 构建后信息 ============
message(STATUS "")
message(STATUS "=== 构建配置摘要 ===")
message(STATUS "Qt版本: ${Qt6_VERSION}")
message(STATUS "链接模式: ${QT_STATIC_BUILD} (OFF=动态, ON=静态)")
message(STATUS "Qt类型: ${QT_TYPE}")
if(WIN32 AND MSVC)
    if(QT_STATIC_BUILD)
        message(STATUS "MSVC运行时: 静态链接 (/MT)")
    else()
        message(STATUS "MSVC运行时: 动态链接 (/MD)")
    endif()
    message(STATUS "优化: /O2 /GL /Gy /LTCG")
    message(STATUS "死代码消除: /OPT:REF /OPT:ICF")
endif()
message(STATUS "=====================")