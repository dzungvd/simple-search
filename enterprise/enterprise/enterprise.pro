#-------------------------------------------------
#
# Project created by QtCreator 2018-03-26T10:46:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = enterprise
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        keymanager.cpp \
    ../../message/message.cpp \
    ../../database/db_interface.cpp \
    ../../vendors/cpp-elasticsearch/src/elasticsearch/elasticsearch.cpp \
    ../../vendors/cpp-elasticsearch/src/http/http.cpp \
    ../../vendors/cpp-elasticsearch/src/json/json.cpp \
    mainwindow_controller.cpp

HEADERS += \
        mainwindow.h \
        keymanager.h \
        ../../vendors/nlohmann_json/nlohmann/json.hpp \
    ../../message/message.h \
    ../../database/db_interface.h \
    ../../vendors/cpp-elasticsearch/src/elasticsearch/elasticsearch.h \
    ../../vendors/cpp-elasticsearch/src/http/http.h \
    ../../vendors/cpp-elasticsearch/src/json/json.h \
    mainwindow_controller.h

FORMS += \
        mainwindow.ui

INCLUDEPATH += ../../vendors/nlohmann_json/ ../../ ../../vendors/cpp-elasticsearch/src/

unix|win32: LIBS += -lzmq

unix|win32: LIBS += -lsodium
