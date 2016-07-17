TEMPLATE = app

QT += qml quick webview sql widgets
CONFIG += c++11
RC_ICONS = icon2.ico

SOURCES += main.cpp src/SqlLiteDatabase.cpp src/NotebooksModel.cpp ext/sqlite3.c src/NotesModel.cpp src/Settings.cpp
HEADERS += include/SqlLiteDatabase.h include/NotesModel.h include/NotebooksModel.h include/Settings.h

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/ext

TRANSLATIONS += translations/pl_PL.ts

lupdate_only{
SOURCES += qml/*.qml
}

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
