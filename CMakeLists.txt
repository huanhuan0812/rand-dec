cmake_minimum_required(VERSION 3.16)

project(rand-dec VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# 设置图标资源（Windows）
if(WIN32)
    set(APP_ICON_RESOURCES "resources.rc")
elseif(APPLE)
    # macOS 使用 .icns 文件，需要额外配置
    # set(MACOSX_BUNDLE_ICON_FILE app.icns)
endif()

qt_add_executable(rand-dec
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${APP_ICON_RESOURCES}
)

target_link_libraries(rand-dec PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# 设置 Windows 应用程序图标
if(WIN32)
    set_target_properties(rand-dec PROPERTIES
        WIN32_EXECUTABLE TRUE
        RC_ICONS "app.ico"
    )
endif()

# macOS 捆绑包设置
set_target_properties(rand-dec PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

# macOS 图标（可选）
if(APPLE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app.icns")
    set_source_files_properties(app.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(rand-dec PRIVATE app.icns)
    set_target_properties(rand-dec PROPERTIES
        MACOSX_BUNDLE_ICON_FILE app.icns)
endif()

include(GNUInstallDirs)
install(TARGETS rand-dec
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)