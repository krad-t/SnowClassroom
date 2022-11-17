cmake_minimum_required(VERSION 3.22)
project(SnowClassroom)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置部分源文件的目录信息
include_directories(
        ./Sources/Headers
        include
        include/vlc)
aux_source_directory(./Source/ SRC_DIR)
link_directories(lib/ffmpeg)
#link_libraries(
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libavcodec.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libavdevice.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libavfilter.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libavformat.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libavutil.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libpostproc.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libswresample.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/ffmpeg/libswscale.dll.a
#)
add_executable(SnowClassroom
#        ${SRC_DIR}
#        Source/Headers/rtmp.hpp
#        Source/Headers/audio_record.hpp
#        Source/Headers/Log.hpp
#        Source/Headers/record_rtmp_test.hpp
        Source/main.cpp
        Source/Headers/XMediaEncode.cpp Source/Headers/XMediaEncode.h Source/Headers/XRtmp.cpp Source/Headers/XRtmp.h Source/Headers/XAudioRecord.cpp Source/Headers/XAudioRecord.h Source/Headers/test.cpp Source/Headers/test.h Source/Headers/XGdigrabScreen.cpp Source/Headers/XGdigrabScreen.h)
## QT
set(CMAKE_PREFIX_PATH D:\\Qt\\Qt5.14.2\\5.14.2\\mingw73_64\\lib\\cmake)
#set(Qt5_DIR D:\\Qt5.9.9\\5.9.9\\mingw53_32\\lib\\cmake\\Qt5)

find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Multimedia
        REQUIRED)

target_link_libraries(SnowClassroom
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Multimedia
        )
target_link_libraries(SnowClassroom
        avfilter
        avutil
        avcodec
        avdevice
        swscale
        swresample
        postproc
        avformat
        )

if (WIN32)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()

link_libraries(
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtCore.dll.a
        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtCored.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtQml.dll.a
        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtQmld.dll.a
#        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtWidgets.dll.a
        E:/Codefield/CODE_C++/Project_Clion/QTProject/SnowClassroom/lib/vlc/libVLCQtWidgetsd.dll.a
)
