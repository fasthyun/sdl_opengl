QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS += -H
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    backup.cpp \
    element3d.cpp \
    font.cpp \
    gltf-loader.cc \
    main.cpp \
    mainwindow.cpp \
    myglwidget.cpp \
    object.cpp \
    particle.cpp \
    zmath.cpp

HEADERS += \
    element3d.h \
    font.h \
    mainwindow.h \
    myglwidget.h \
    object.h \
    zmath.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
#
INCLUDEPATH += /usr/include/freetype2
INCLUDEPATH += /usr/include/libpng16
#INCLUDEPATH += C:/boost/boost_1_53_0/
LIBS +=   -lboost_filesystem -lboost_system -lfreetype -lGLU
#LIBS += "-LC:/boost/boost_1_53_0/stage/lib/"
