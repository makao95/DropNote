#include "NotesModel.h"
#include <QSqlQuery>
#include "SqlLiteDatabase.h"
#include <QDebug>
#include <QSqlError>

const char* NotesModel::COLUMN_NAMES[] = {
    "id",
    "title",
    "text_data",
    "created",
    "edited",
    //    NULL
};

const char* NotesModel::SQL_SELECT =
        "SELECT notes.id, notes.title, notes.text_data, notes.created, notes.edited"
        " FROM notes";

NotesModel::NotesModel(QObject* parent, QSqlDatabase& db):QSqlQueryModel(parent), db(db), currentNotebook(0){
    db = QSqlDatabase::database("localdatabase");

    if (!db.isOpen())
        qDebug() << "WHAT?";
    QSqlQuery caseSensitivityQuery("PRAGMA case_sensitive_like=OFF", db);
    caseSensitivityQuery.exec();
    prepareDatabase();

    int idx = 0;
    while(COLUMN_NAMES[idx]) {
        roles[Qt::UserRole + idx + 1] = COLUMN_NAMES[idx];
        idx++;
    }
    refresh();
}

void NotesModel::prepareDatabase(){
    auto db = QSqlDatabase::database("localdatabase");
    std::vector<QString> queries = {"CREATE TABLE IF NOT EXISTS notes(id INTEGER PRIMARY KEY ASC, notebook_id INT, created DATETIME, edited DATETIME, title varchar(255), text_data TEXT);"};
    QSqlQuery query(db);
    for (auto&& q : queries){
        query.prepare(q);
        query.exec();
    }
}

QVariant NotesModel::data(const QModelIndex &index, int role) const{
    QVariant value = QSqlQueryModel::data(index, role);
    if(role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    return value;
}


void NotesModel::refresh(){
    auto db = QSqlDatabase::database("localdatabase");
    QString order;
    sortDescending ? order = "DESC" : order = "ASC";
    if (currentNotebook == 0)
        this->setQuery(SQL_SELECT + QString(" ORDER BY created ") + order, db);
    else{
        QSqlQuery query(db);
        query.prepare("SELECT id, title, text_data, created, edited FROM notes WHERE notebook_id=:notebook ORDER BY created " + order);
        query.bindValue(":notebook", currentNotebook);
        query.exec();
        this->setQuery(query);
    }
}

QHash<int, QByteArray> NotesModel::roleNames() const{
    return roles;
}


bool NotesModel::doesDayHaveNote(QDate date){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("SELECT id FROM notes WHERE DATE(created)=:date");
    query.bindValue(":date", date);
    if(!query.exec())
        qDebug() << query.lastError().text();
    return query.next();
}

void NotesModel::update(int id, QString title, QString text){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("UPDATE notes SET title=:title, text_data=:text, edited=DATE('now') WHERE id=:id");
    query.bindValue(":title", title);
    query.bindValue(":text", text);
    query.bindValue(":id", id);
    if (!query.exec())
        qDebug() << query.lastError().text();
    refresh();
}

int NotesModel::addNote(int notebook_id, QString title, QString text){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("INSERT INTO notes VALUES(NULL, :notebook_id, DATETIME('now', 'localtime'), DATETIME('now', 'localtime'), :title, :text_data)");
    query.bindValue(":notebook_id", notebook_id);
    query.bindValue(":title", title);
    query.bindValue(":text_data", text);
    if(!query.exec())
        qDebug() << sqlite::getLastExecutedQuery(query);
    refresh();
    return 0;
}

bool NotesModel::deleteNote(int id){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("DELETE FROM notes WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    refresh();
    return true;
}

void NotesModel::setSearch(QString s){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("SELECT notes.id, notes.title, notes.text_data, notes.created, notes.edited FROM notes WHERE title LIKE :s OR text_data LIKE :s");
    query.bindValue(0, QString("%") + s + QString('%'));
    query.bindValue(1, QString("%") + s + QString('%'));
    query.exec();
    setQuery(query);
}

void NotesModel::setCurrentNotebook(int id){
    currentNotebook = id;
    refresh();
}

int NotesModel::getCurrentNotebook(){
    return currentNotebook;
}

void NotesModel::setSortDescending(bool b){
    sortDescending = b;
    refresh();
}

bool NotesModel::getSortDescending(){
    return sortDescending;
}
