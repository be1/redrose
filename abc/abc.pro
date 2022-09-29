include(../common.pri)
TEMPLATE = lib
TARGET = abc
CONFIG -= qt
CONFIG += link_pkgconfig debug staticlib
PKGCONFIG +=
SOURCES += abc.c parser.c
HEADERS += abc.h parser.h

#PARSER = parser.peg
#packcc.depends = $$PARSER
#packcc.input = PARSER
#packcc.output += ${QMAKE_FILE_BASE}.h
#packcc.variable_out = HEADERS
#packcc.commands = packcc ${QMAKE_FILE_IN}
#QMAKE_EXTRA_COMPILERS += packcc
DISTFILES += parser.peg
