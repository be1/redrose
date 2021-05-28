VERSION = 0.1
REVISION = $$system(git describe --long --tags 2>/dev/null || echo "stable")
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(BINDIR): BINDIR = $$PREFIX/bin
isEmpty(DATADIR): DATADIR = $$PREFIX/share
ABCM2PSDIR = $$DATADIR/qabc/abcm2ps
