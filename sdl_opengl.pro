QT      -= gui core qtwidget
CONFIG  += c++11 debug
CONFIG  -= qml_debug

QMAKE_CXXFLAGS += -H -Wno-unused-parameter -Wno-unused-variable

INCLUDEPATH += /usr/include/freetype2
LIBS +=  -lSDL2 -lboost_filesystem -lboost_system -lfreetype -lGL -lGLU -lGLEW

DEFINES +=  #Qt5 #되네

TARGET = main

SOURCES += \
    Shader.cpp \
    backup.cpp \
    console.cpp \
    element3d.cpp \
    font.cpp \
    gltf-loader.cc \
    main_sdl.cpp \
    object.cpp \
    particle.cpp \
    texture.cpp \
    zmath.cpp

HEADERS += \
    Shader.h \
    console.h \
    element3d.h \
    font.h \
    gltext.h \
    object.h \
    texture.h \
    zmath.h
