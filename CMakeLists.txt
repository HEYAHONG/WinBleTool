

cmake_minimum_required(VERSION 3.8)
#程序名称
Set(PROJECT_NAME WinBleTool)
#主版本
set(PROJECT_MAJOR_VERSION_STR 1)
#次版本号
set(PROJECT_MINOR_VERSION_STR 0)
#修订号
set(PROJECT_REVISION_VERSION_STR 0)
#构建版本（以时间戳天数作为构建版本）
string(TIMESTAMP PROJECT_BUILD_TIME_SECOND "%s" UTC)
math(EXPR  PROJECT_BUILD_VERSION_STR "${PROJECT_BUILD_TIME_SECOND}/60/60/24"  OUTPUT_FORMAT DECIMAL)
#程序版本
set(PROJECT_VERSION_STR "${PROJECT_MAJOR_VERSION_STR}.${PROJECT_MINOR_VERSION_STR}.${PROJECT_REVISION_VERSION_STR}.${PROJECT_BUILD_VERSION_STR}")
message(STATUS "Version:${PROJECT_VERSION_STR}")
configure_file(Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Version.h)
if(WIN32)
configure_file(Version.h.in ${CMAKE_CURRENT_SOURCE_DIR}/win32/Version.h)
endif()

Project(${PROJECT_NAME}	VERSION "${PROJECT_VERSION_STR}")
Project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

option(STATIC_BUILD "static build" OFF)



if(WIN32)
	if(MSVC)
		add_compile_options(-utf-8 )
	else()
		set(MAIN_LIBS "-static-libgcc -static-libstdc++")
	endif()

else()
	message(FATAL_ERROR "only support windows")
endif()

if(STATIC_BUILD)
	if(NOT MSVC)
		set(MAIN_LIBS "${MAIN_LIBS} -static")
	endif()
endif()


file(GLOB MAIN_CXX_C_FILES *.cpp *.hpp *.c *.h bluetooth/*.c bluetooth/*.cpp bluetooth/*.h )
list(APPEND  MAIN_SRCS ${MAIN_CXX_C_FILES})

if(WIN32)
	file(GLOB_RECURSE MAIN_RC_FILES win32/*.rc)
	list(APPEND  MAIN_SRCS ${MAIN_RC_FILES})
endif()

#只支持Win10及以上系统,且必须使用UNICODE
add_definitions("-D_WIN32_WINNT=_WIN32_WINNT_WIN10"  "-DINITGUID")

add_executable(${PROJECT_NAME} WIN32 ${MAIN_SRCS})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC bluetooth )

#添加线程库
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})

#添加wxWidgets库
if(STATIC_BUILD)
        set(wxWidgets_USE_STATIC ON)
endif()
if(WIN32)
	find_package(wxWidgets 3.1.0 REQUIRED core base adv aui webview html gl richtext )
else()
	find_package(wxWidgets 3.1.0 REQUIRED all)
endif()
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})


#添加WinBle库
add_subdirectory(WinBle/src  EXCLUDE_FROM_ALL SYSTEM)
Set(PROJECT_NAME WinBleTool)
target_link_libraries(${PROJECT_NAME} WinBle)

include(GNUInstallDirs)
#安装
INSTALL(TARGETS ${PROJECT_NAME}
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")

#打包NSIS安装包
message(STATUS "this is a windows system")
set(CPACK_PACKAGE_NAME  "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION_STR}")
set(CPACK_PACKAGE_VENDOR "HEYAHONG")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "WinBleTool By HYH")
set(CPACK_PACKAGE_DESCRIPTION "WinBleTool By HYH")
set(CPACK_PACKAGE_CONTACT    "2229388563@qq.com")
#设置安装程序图标
set(CPACK_PACKAGE_ICON_PATH_STRING "${CMAKE_CURRENT_SOURCE_DIR}/win32/window_resources.ico")
string(REPLACE "/" "\\\\" CPACK_PACKAGE_ICON ${CPACK_PACKAGE_ICON_PATH_STRING})
set(CPACK_NSIS_MUI_ICON ${CPACK_PACKAGE_ICON})
set(CPACK_NSIS_MUI_UNIICON ${CPACK_PACKAGE_ICON})
#安装前卸载
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
#设置在控制面板的名称
set(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_NAME})
#设置需要创建快捷方式的程序
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME};${PROJECT_NAME} )
#设置LICENSE
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/LICENSE.txt)
#设置Welcome
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/WELCOME.txt)
#设置readme
set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/win32/nsis/README.txt)
set(CPACK_GENERATOR "NSIS")
include(CPack)

endif()

