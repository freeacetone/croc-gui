QT += core widgets network gui sql

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    database.cpp \
    g.cpp \
    licensewindow.cpp \
    main.cpp \
    socketchecker.cpp \
    widget.cpp

HEADERS += \
    database.h \
    g.h \
    licensewindow.h \
    resultpair.h \
    socketchecker.h \
    widget.h

FORMS += \
    licensewindow.ui \
    widget.ui

RESOURCES += \
    files.qrc
