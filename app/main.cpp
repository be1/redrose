#include "config.h"
#include "AbcApplication.h"
#include "AbcMainWindow.h"
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char** argv)
{
	Q_INIT_RESOURCE(resources);

	AbcApplication abcapplication(argc, argv);

	QString locale = QLocale::system().name();
	QTranslator qtTranslator;
	qtTranslator.load("qt_" + locale,
			QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	abcapplication.installTranslator(&qtTranslator);

	QTranslator qabcTranslator;
	if (!qabcTranslator.load(TARGET "_" + locale, "locale"))
		qabcTranslator.load(TARGET "_" + locale, DATADIR "/" TARGET "/locale");
	abcapplication.installTranslator(&qabcTranslator);

	QCommandLineParser parser;
	parser.setApplicationDescription("ABC music notation minimal GUI.");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("score", QCoreApplication::translate("main", "ABC score file to work on."));

	parser.process(abcapplication);

	AbcMainWindow w;
	abcapplication.setMainWindow(&w);

    QString iconpath = QString(DATADIR "/icons/hicolor/scalable/apps/" TARGET ".svg");
	if (QFileInfo::exists(iconpath))
		abcapplication.setWindowIcon(QIcon(iconpath));

	const QStringList args = parser.positionalArguments();
	if (args.length() > 0)
		abcapplication.openFileNames(args);

	return abcapplication.exec();
}
