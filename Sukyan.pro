#-------------------------------------------------
#
# Project created by QtCreator 2019-02-18T14:00:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sukyan
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dockimage.cpp \
    dockselection.cpp \
    dockexplorer.cpp \
    image.cpp \
    view.cpp \
    scene.cpp \
    dockdrawing.cpp \
    polygon.cpp \
    polygonslist.cpp \
    marong.cpp \
    contrast.cpp \
    copypolygonsdialog.cpp \
    splitdialog.cpp

HEADERS += \
        mainwindow.h \
    dockimage.h \
    dockselection.h \
    dockexplorer.h \
    image.h \
    scene.h \
    view.h \
    core.h \
    dockdrawing.h \
    polygon.h \
    polygonslist.h \
    marong.h \
    contrast.h \
    copypolygonsdialog.h \
    splitdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

RC_ICONS = icon.ico
