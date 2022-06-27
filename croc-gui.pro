QT += core widgets network gui sql

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    crocexecutetester.cpp \
    database.cpp \
    g.cpp \
    informationwindow.cpp \
    licensewindow.cpp \
    main.cpp \
    socketchecker.cpp \
    widget.cpp

HEADERS += \
    crocexecutetester.h \
    database.h \
    g.h \
    informationwindow.h \
    licensewindow.h \
    resultpair.h \
    socketchecker.h \
    widget.h

FORMS += \
    informationwindow.ui \
    licensewindow.ui \
    widget.ui

RESOURCES += \
    files.qrc
