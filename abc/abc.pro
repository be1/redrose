include(../common.pri)
TEMPLATE = lib
TARGET = abc
CONFIG -= qt
CONFIG += link_pkgconfig debug staticlib
DEFINES += _FORTIFY_SOURCE=2
QMAKE_CFLAGS += -O2
PKGCONFIG +=
SOURCES += abc.c parser.c
HEADERS += abc.h parser.h
