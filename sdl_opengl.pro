QT      -= gui core qtwidget
CONFIG  += c++11 warn_off
CONFIG  -= qml_debug debug

QMAKE_CXXFLAGS += -H -Wno-unused-parameter -Wno-unused-variable -Wwrite-strings -Wunused-but-set-variable -Wunused-function
QMAKE_CFLAGS += -Wunused-function

#INCLUDEPATH += /usr/include/freetype2
LIBS +=  -lSDL2 -lSDL2_image -lassimp -lboost_filesystem -lboost_system  -lGL -lGLU -lGLEW
#QMAKE_LIBDIR += ./
# -lfreetype
DEFINES +=  #Qt5 #되네

#QMAKE_PRE_LINK = gcc -shared -o libgltext.so gltext.o

TARGET = main

SOURCES += \
    Shader.cpp \
    backup.cpp \
    console.cpp \
    element3d.cpp \
    gltext.c \
    main.cpp \
    misc.c \
    model.cpp \
    object.cpp \
    particle.cpp \
    texture.cpp \
    zmath.cpp

HEADERS += \
    Shader.h \
    console.h \
    element3d.h \
    font.h \    
    model.h \
    object.h \
    texture.h \
    zmath.h

DISTFILES += \
    shader/Sprite.frag \
    shader/Sprite.vert \
    shader/basic_fragment.glsl \
    shader/basic_vertex.glsl \
    shader/fshader.glsl \
    shader/grid_fragment.glsl \
    shader/grid_vertex.glsl \
    shader/texture_fragment.glsl \
    shader/texture_vertex.glsl \
    shader/vertex.glsl \
    shader/vshader.glsl \
    shader/basic_fragment120.glsl \
    shader/basic_vertex120.glsl
