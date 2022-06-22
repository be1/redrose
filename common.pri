VERSION = 0.2.7
REVISION = $$system(git describe --long --tags 2>/dev/null || echo "stable")
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT _FORTIFY_SOURCE=2
}
CONFIG(debug, debug|release) {
    DEFINES += EBUG
    QMAKE_CXXFLAGS += -rdynamic
    QMAKE_CFLAGS += -rdynamic
}
isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(BINDIR): BINDIR = $$PREFIX/bin
isEmpty(DATADIR): DATADIR = $$PREFIX/share
ABCM2PSDIR = $$DATADIR/redrose/abcm2ps
