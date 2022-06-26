QT += core widgets network gui sql

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    database.cpp \
    g.cpp \
    main.cpp \
    socketchecker.cpp \
    widget.cpp

HEADERS += \
    database.h \
    g.h \
    resultpair.h \
    socketchecker.h \
    widget.h

FORMS += \
    widget.ui

RESOURCES += \
    files.qrc
