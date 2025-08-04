cmake_minimum_required(VERSION 3.19)
project(dirloc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(dirloc
    main.cpp
    dirloc.hh dirloc.cpp
    MainWindow.hh MainWindow.cpp
)

target_link_libraries(dirloc
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(dirloc PRIVATE Qt6::Widgets)

include(GNUInstallDirs)

install(TARGETS dirloc
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
