include(../common.pri)
TEMPLATE = lib
TARGET = abc
CONFIG -= qt
CONFIG += link_pkgconfig debug staticlib
PKGCONFIG +=
SOURCES += abc.c parser.c
HEADERS += abc.h parser.h
