

cmake_minimum_required(VERSION 3.8)

Set(PROJECT_NAME WinBleTool)

Project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

option(STATIC_BUILD "static build" OFF)



if(WIN32)
	set(MAIN_LIBS "-static-libgcc -static-libstdc++")
else()
	message(FATAL_ERROR "only support windows")
endif()

if(STATIC_BUILD)
	set(MAIN_LIBS "${MAIN_LIBS} -static")
endif()


file(GLOB MAIN_CXX_C_FILES *.cpp *.hpp *.c *.h bluetooth/*.c bluetooth/*.cpp bluetooth/*.h )
list(APPEND  MAIN_SRCS ${MAIN_CXX_C_FILES})

if(WIN32)
	file(GLOB_RECURSE MAIN_RC_FILES win32/*.rc)
	list(APPEND  MAIN_SRCS ${MAIN_RC_FILES})
endif()


add_executable(${PROJECT_NAME} ${MAIN_SRCS})
target_link_libraries(${PROJECT_NAME} ${MAIN_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC bluetooth )

#添加线程库
FIND_PACKAGE(Threads REQUIRED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${CMAKE_THREAD_LIBS_INIT})

#添加wxWidgets库
if(STATIC_BUILD)
        set(wxWidgets_USE_STATIC ON)
endif()
find_package(wxWidgets REQUIRED all )
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

#只支持Win10及以上系统,且必须使用UNICODE
add_definitions("-D_WIN32_WINNT=_WIN32_WINNT_WIN10" "-DUNICODE" "-DINITGUID")

#添加WinBle库
add_subdirectory(WinBle/src)
Set(PROJECT_NAME WinBleTool)
target_link_libraries(${PROJECT_NAME} WinBle)

include(GNUInstallDirs)
#安装
INSTALL(TARGETS ${PROJECT_NAME}
       RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
       LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
       ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
