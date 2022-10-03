include(../common.pri)
QT += core widgets gui svg printsupport
equals(QT_MAJOR_VERSION, 6) {
    QT += svgwidgets core5compat
}
TEMPLATE = app
CONFIG += link_pkgconfig
PKGCONFIG += fluidsynth drumstick-file
DEFINES += USE_LIBABCM2PS
TARGET = redrose
RESOURCES += resources.qrc
DISTFILES +=  dict.txt config.h.in
SOURCES = ScoreMenu.cpp PreferencesMenu.cpp HelpMenu.cpp AbcPlainTextEdit.cpp PlayPushButton.cpp EditVBoxLayout.cpp EditWidget.cpp EditTabWidget.cpp RunPushButton.cpp AbcMainWindow.cpp AbcApplication.cpp main.cpp \
	AbcProcess.cpp \
	AbcTemporaryFile.cpp \
	MainHSplitter.cpp \
	PlayerThread.cpp \
	ScoreSvgWidget.cpp \
	ViewVSplitter.cpp \
	abcsmf.cpp \
	abcsynth.cpp \
	editorprefdialog.cpp \
	generator.cpp \
	midigenerator.cpp \
	psgenerator.cpp \
	settings.cpp \
	sfloader.cpp \
	QProgressIndicator.cpp \
	svggenerator.cpp
HEADERS = ScoreMenu.h PreferencesMenu.h HelpMenu.h AbcPlainTextEdit.h PlayPushButton.h EditVBoxLayout.h EditWidget.h EditTabWidget.h RunPushButton.h AbcMainWindow.h AbcApplication.h \
	AbcProcess.h \
	AbcTemporaryFile.h \
	MainHSplitter.h \
	PlayerThread.h \
	ScoreSvgWidget.h \
	ViewVSplitter.h \
	abcsmf.h \
	abcsynth.h \
	config.h \
	editorprefdialog.h \
	generator.h \
	midigenerator.h \
	psgenerator.h \
	settings.h \
	sfloader.h \
	QProgressIndicator.h \
	svggenerator.h
config.input = config.h.in
config.output = config.h
QMAKE_SUBSTITUTES += config
LIBS += ../abcm2ps/libabcm2ps.a ../abc/libabc.a
isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
TRANSLATIONS += $${TARGET}_en.ts $${TARGET}_fr.ts
LOCALE_DIR = locale
updateqm.input = TRANSLATIONS
updateqm.output = $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm $$LOCALE_DIR/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += updateqm
target.path = $$BINDIR
translations.path = $$DATADIR/$$TARGET
translations.files = $$LOCALE_DIR
desktop.path = $$DATADIR/applications
desktop.files = redrose.desktop
icon.path = $$DATADIR/icons/hicolor/scalable/apps
icon.files = redrose.svg
mime.path = $$DATADIR/mime/packages
mime.files = redrose.xml
metainfo.path = $$DATADIR/metainfo
metainfo.files = fr.free.brouits.redrose.metainfo.xml
INSTALLS += target translations desktop icon mime metainfo

