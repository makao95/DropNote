#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <SqlLiteDatabase.h>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QQmlContext>
#include <QTranslator>
#include "NotesModel.h"
#include "NotebooksModel.h"
#include "Settings.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    QTranslator polishTranslator;
    polishTranslator.load("translations/pl_PL");

    app.installTranslator(&polishTranslator);

    QSqlDatabase *database = new QSqlDatabase();
    *database = QSqlDatabase::addDatabase("QSQLITE", "localdatabase");
    database->setDatabaseName("dropnote.db");

    NotesModel notesModel(&app, *database);
    NotebooksModel notebooksModel(&app, *database);
    Settings settings(database);

    QQmlApplicationEngine engine;

    auto context = engine.rootContext();
    context->setContextProperty("notesModel", &notesModel);
    context->setContextProperty("notebooksModel", &notebooksModel);
    context->setContextProperty("settings", &settings);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
