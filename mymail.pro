QT += core gui widgets webenginewidgets
CONFIG += c++17
CONFIG += link_pkgconfig

# This seems to fix link errors caused by webengine
CONFIG+=use_gold_linker

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    messageview.cpp

HEADERS += \
    mainwindow.h \
    messageview.h

FORMS += \
    mainwindow.ui

LIBS += -lnotmuch

PKGCONFIG += fmt
PKGCONFIG += gmime-3.0
