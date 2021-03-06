TEMPLATE = app

QT += widgets

CONFIG += c++11

SOURCES += main.cpp \
    bbswitchchecker.cpp \
    decision.cpp \
    bbswitcher.cpp \
    runGuard.cpp \
    log.cpp \
    battlenetchecker.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    bbswitchchecker.hpp \
    decision.hpp \
    bbswitcher.hpp \
    runGuard.hpp \
    log.h \
    battlenetchecker.hpp

FORMS += \
    log.ui
