QT += core widgets network gui sql

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    croccliprocess.cpp \
    crocexecutetester.cpp \
    database.cpp \
    g.cpp \
    informationwindow.cpp \
    latestversiongithubchecker.cpp \
    licensewindow.cpp \
    main.cpp \
    socketchecker.cpp \
    version.cpp \
    widget.cpp

HEADERS += \
    croccliprocess.h \
    crocexecutetester.h \
    database.h \
    g.h \
    informationwindow.h \
    latestversiongithubchecker.h \
    licensewindow.h \
    resultpair.h \
    socketchecker.h \
    version.h \
    widget.h

FORMS += \
    informationwindow.ui \
    licensewindow.ui \
    widget.ui

RESOURCES += \
    files.qrc
