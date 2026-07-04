cmake_minimum_required(VERSION 3.16)

project(rand-dec VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
        
        # 静态链接MSVC运行时库
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
        
        # 设置生成调试信息（PDB）
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
        set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    endif()
    
    # Windows静态链接Qt
    set(BUILD_SHARED_LIBS OFF)
    set(QT_STATIC_BUILD ON)
endif()

# ============ 查找Qt ============
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# ============ 资源文件 ============
if(WIN32)
    set(APP_ICON_RESOURCES "resources.rc")
elseif(APPLE)
    # macOS 使用 .icns 文件，需要额外配置
    set(MACOSX_BUNDLE_ICON_FILE app.icns)
endif()

# ============ 可执行文件 ============
qt_add_executable(rand-dec
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${APP_ICON_RESOURCES}
)

# ============ 链接库 ============
# Qt核心库
target_link_libraries(rand-dec PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)

# ============ 平台特定链接 ============
if(WIN32)
    # Windows：静态链接Qt插件
    set(QT_REQUIRED_PLUGINS
        Qt6::QWindowsIntegrationPlugin
    )
    
    # 如果使用静态Qt，需要链接插件
    if(QT_STATIC_BUILD)
        target_link_libraries(rand-dec PRIVATE 
            ${QT_REQUIRED_PLUGINS}
        )
    endif()
    
    # Windows系统库（静态链接时必需）
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
    
    # macOS 图标（可选）
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