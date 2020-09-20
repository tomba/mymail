QT += core gui widgets webenginewidgets
CONFIG += c++17

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
LIBS += -lfmt
