# 打包核心代码
file(GLOB_RECURSE CORE_FILES "core/*.cpp")
add_library(${CMAKE_PROJECT_NAME}_core ${CORE_FILES})

# 配置 QT5
set(CMAKE_PREFIX_PATH C:/Qt/5.11.2/mingw53_32)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()
find_package(Qt5Widgets REQUIRED)

# 配置 QRC
qt5_add_resources(QRCS )

# 打包UI文件
file(GLOB_RECURSE UI_FILES "view/*.ui")
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# 打包视图代码
file(GLOB_RECURSE VIEW_FILES "view/*.cpp")

# 添加可执行文件
add_executable(${CMAKE_PROJECT_NAME}_run Main.cpp ${VIEW_FILES} ${UI_HEADERS} ${QRCS})

# 添加所有依赖
target_link_libraries(${CMAKE_PROJECT_NAME}_run Qt5::Widgets ${CMAKE_PROJECT_NAME}_core)
