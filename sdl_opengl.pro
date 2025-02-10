QT      -= gui core qtwidget
CONFIG  += c++11 precompile_header #warn_off
CONFIG  -= qml_debug debug
#LANGUAGE = C++

QMAKE_CXXFLAGS += -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wno-unused-value -Wwrite-strings -Wno-unused-but-set-variable -Wunused-function
#QMAKE_CFLAGS += -H -Wunused-function -Wno-unused-variable

#INCLUDEPATH += /usr/include/freetype2
LIBS +=  -lSDL2 -lSDL2_image -lassimp -lboost_filesystem -lboost_system  -lGL -lGLU -lGLEW
#QMAKE_LIBDIR += ./
# -lfreetype

PRECOMPILED_HEADER = stable.h
#nuklear.h nuklear_sdl_gl3.h

precompile_header:!isEmpty(PRECOMPILED_HEADER) {
DEFINES += USING_PCH
}

DEFINES +=  #Qt5 #되네

#QMAKE_PRE_LINK = gcc -shared -o libgltext.so gltext.o

TARGET = main

SOURCES += \
    Shader.cpp \
    backup.cpp \
    console.cpp \
    element3d.cpp \
    nuklear.cpp \
    main.cpp \    
    model.cpp \
    object.cpp \
    particle.cpp \
    texture.cpp \
    xmath.cpp
# gltext.c misc.c

HEADERS += \
    Shader.h \
    console.h \
    element3d.h \
    font.h \
    model.h \
    object.h \
    texture.h \
    xmath.h \
    stable.h

DISTFILES += \
    shader/Sprite.frag \
    shader/Sprite.vert \
    shader/basic_fragment.glsl \
    shader/basic_vertex.glsl \
    shader/color_fragment.glsl \
    shader/color_vertex.glsl \
    shader/fshader.glsl \
    shader/grid_fragment.glsl \
    shader/grid_vertex.glsl \
    shader/point_fragment.glsl \
    shader/point_vertex.glsl \
    shader/texture_fragment.glsl \
    shader/texture_vertex.glsl \
    shader/basic_fragment120.glsl \
    shader/basic_vertex120.glsl
