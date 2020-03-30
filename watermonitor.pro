#-------------------------------------------------
#
# Project created by QtCreator 2020-03-28T11:36:54
#
#-------------------------------------------------

QT       += core gui sql net

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += NO_PROFILE
TARGET = cmplatform1
TEMPLATE = app


SOURCES += main.cpp\
        funwindow.cpp \
    systemwindow.cpp \
    loginwindow.cpp \
    iprotocol.cpp \
    itask.cpp \
    nh3ntask.cpp \
    elementinterface.cpp \
    elementfactory.cpp \
    protocolv1.cpp \
    profile.cpp \
    common.cpp

HEADERS  += funwindow.h \
    systemwindow.h \
    loginwindow.h \
    iprotocol.h \
    itask.h \
    nh3ntask.h \
    elementinterface.h \
    elementfactory.h \
    protocolv1.h \
    profile.h \
    common.h

FORMS    += funwindow.ui \
    systemwindow.ui \
    loginwindow.ui
