#-------------------------------------------------
#
# Project created by QtCreator 2020-03-28T11:36:54
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(hardwareinterface/hardwareinterface.pri)
include(modbus/modbus.pri)
include(env.pri)

DESTDIR = $${PWD}/dist
TARGET = cmplat
TEMPLATE = app

win32 {
DEFINES += NO_STEP_CHECK
}


SOURCES += main.cpp\
    systemwindow.cpp \
    iprotocol.cpp \
    itask.cpp \
    nh3ntask.cpp \
    elementinterface.cpp \
    elementfactory.cpp \
    protocolv1.cpp \
    profile.cpp \
    common.cpp \
    instructioneditor.cpp \
    qfmain.cpp \
    login/userdlg.cpp \
    login/md5.cpp \
    login/loginmanage.cpp \
    modbusmodule.cpp \
    querydata.cpp \
    calibframe.cpp \
    datafit.cpp \
    keyboard/keyboardenter.cpp \
    keyboard/keyboard.cpp \
    screensaver.cpp

HEADERS  += systemwindow.h \
    iprotocol.h \
    itask.h \
    nh3ntask.h \
    elementinterface.h \
    elementfactory.h \
    protocolv1.h \
    profile.h \
    common.h \
    instructioneditor.h \
    qfmain.h \
    login/userdlg.h \
    login/md5.h \
    login/loginmanage.h \
    modbusmodule.h \
    defines.h \
    querydata.h \
    calibframe.h \
    datafit.h \
    keyboard/keyboardenter.h \
    keyboard/keyboard.h \
    screensaver.h

FORMS    += systemwindow.ui \
    qfmain.ui \
    login/userdlg.ui \
    modbusmodule.ui \
    setui.ui \
    querydata.ui \
    calibframe.ui \
    maintaince.ui \
    measuremode.ui \
    keyboard/keyboard.ui \
    screensaver.ui \
    lightvoltage.ui

RESOURCES += res/qtres.qrc
