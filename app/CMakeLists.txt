configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

find_package(Qt6 QUIET COMPONENTS Core) # test only
if (NOT Qt6_FOUND OR ForceQt5)
	message("Qt6 not found, looking for Qt5")
	find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport LinguistTools)
	set(QT_VERSION_MAJOR 5)
else()
	find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport LinguistTools)
	set(QT_VERSION_MAJOR 6)
endif()

message("Qt version major is ${QT_VERSION_MAJOR}")
find_package(Smf QUIET)
if (NOT Smf_FOUND)
	pkg_search_module(Smf REQUIRED smf)
endif()
message("Smf version is ${Smf_VERSION}")
pkg_search_module(Spectre REQUIRED libspectre)
message("Spectre version is ${Spectre_VERSION}")
pkg_search_module(Fluidsynth REQUIRED fluidsynth>2.0)
message("Fluidsynth version is ${Fluidsynth_VERSION}")


if (NOT Qt6_FOUND)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
	set(CMAKE_AUTOUIC ON)
else()
	qt_standard_project_setup(
		I18N_SOURCE_LANGUAGE en
		I18N_TRANSLATED_LANGUAGES en fr pt
	)
endif()

set(SOURCES
	AbcApplication.cpp
	AbcMainWindow.cpp
	AbcPlainTextEdit.cpp
	AbcProcess.cpp
	abcsmf.cpp
	abcsynth.cpp
	AbcTemporaryFile.cpp
	editorprefdialog.cpp
	EditTabWidget.cpp
	EditVBoxLayout.cpp
	EditWidget.cpp
	generator.cpp
	HelpMenu.cpp
	logwindow.cpp
	main.cpp
	MainHSplitter.cpp
	midigenerator.cpp
	navmenu.cpp
	playerprefdialog.cpp
	PlayPushButton.cpp
	PreferencesMenu.cpp
	psgenerator.cpp
	psprefdialog.cpp
	QProgressIndicator.cpp
	RunPushButton.cpp
	ScoreMenu.cpp
	scorepswidget.cpp
	settings.cpp
	sfloader.cpp
	ViewVSplitter.cpp
	wizard.cpp
)

set(TS_FILES redrose_en.ts redrose_fr.ts redrose_pt.ts)
set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/locale")
if (NOT Qt6_FOUND)
	qt5_add_resources(SOURCES resources.qrc)
	qt5_add_translation(QM_FILES ${TS_FILES})
	add_executable(redrose ${SOURCES} ${QM_FILES})
elseif (Qt6_VERSION VERSION_LESS 6.7)
	qt_add_resources(SOURCES resources.qrc)
	qt_add_executable(redrose ${SOURCES})
	qt_add_translations(redrose TS_FILES ${TS_FILES})
else()

	qt_add_resources(SOURCES resources.qrc)
	qt_add_executable(redrose ${SOURCES})
	qt_add_translations(redrose IMMEDIATE_CALL)
endif()

target_compile_options(redrose PUBLIC
	${Smf_CFLAGS}
)
target_compile_definitions(redrose PUBLIC
	$<$<CONFIG:DEBUG>:EBUG>
	$<$<CONFIG:RELEASE>:QT_NO_DEBUG_OUTPUT>
)

set_target_properties(redrose PROPERTIES
	WIN32_EXECUTABLE ON
	MACOSX_BUNDLE ON
)

target_link_libraries(redrose PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::PrintSupport fluidsynth smf spectre abc ${CMAKE_DL_LIBS})

install(TARGETS redrose DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/locale DESTINATION ${PROJECT_DATADIR}/redrose)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/redrose.svg DESTINATION ${PROJECT_DATADIR}/icons/hicolor/scalable/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/redrose.xml DESTINATION ${PROJECT_DATADIR}/mime/packages)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/fr.free.brouits.redrose.metainfo.xml DESTINATION ${PROJECT_DATADIR}/metainfo)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/redrose.desktop DESTINATION ${PROJECT_DATADIR}/applications)
