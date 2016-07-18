#include "NotebooksModel.h"
#include <QSqlQuery>
#include "SqlLiteDatabase.h"
#include <QDebug>
#include <QSqlError>

const char* NotebooksModel::COLUMN_NAMES[] = {
    "id",
    "name",
    "parent_id",
    //    NULL
};

const char* NotebooksModel::SQL_SELECT =
        "SELECT id, name, parent_id"
        " FROM notebooks";

NotebooksModel::NotebooksModel(QObject* parent, QSqlDatabase& db):QSqlQueryModel(parent), db(db){
    db = QSqlDatabase::database("localdatabase");
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

void NotebooksModel::prepareDatabase(){
    std::vector<QString> queries = {"CREATE TABLE IF NOT EXISTS notebooks(id INTEGER PRIMARY KEY ASC, name VARCHAR(255), parent_id INT);"};
    {
        QSqlQuery query(db);
        for (auto&& q : queries){
            query.prepare(q);
            query.exec();
        }
    }
    //create 'All' Notebook if table is empty
    QSqlQuery query(db);
    query.prepare("SELECT id FROM notebooks");
    query.exec();
    if (!query.next()){
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO notebooks VALUES(0, 'All' , NULL)");
        insertQuery.exec();
    }
}

QVariant NotebooksModel::data(const QModelIndex &index, int role) const{
    QVariant value = QSqlQueryModel::data(index, role);
    if(role < Qt::UserRole) {
        value = QSqlQueryModel::data(index, role);
    } else {
        int columnIdx = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), columnIdx);
        value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }
    if (value=="All")
        return tr("All", "notebooks");
    return value;
}


void NotebooksModel::refresh(){
    this->setQuery(SQL_SELECT, db);
}

QHash<int, QByteArray> NotebooksModel::roleNames() const{
    return roles;
}

bool NotebooksModel::doesDayHaveNote(int id, QDate date){
    QSqlQuery query(db);
    if (id == 0)
        query.prepare("SELECT id FROM notes WHERE DATE(created)=:date");
    else
        query.prepare("SELECT id FROM notes WHERE DATE(created)=:date AND notebook_id=:id");
    query.bindValue(":date", date);
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << query.lastError().text();
    return query.next();
}

void NotebooksModel::update(int id, QString title){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("UPDATE notes SET title=:title, text_data=:text, edited=DATE('now') WHERE id=:id");
    query.bindValue(":title", title);
    query.bindValue(":id", id);
    if (!query.exec())
        qDebug() << query.lastError().text();
    refresh();
}

int NotebooksModel::addNotebook(int parent_notebook, QString name){
    QSqlQuery query(db);
    query.prepare("INSERT INTO notebooks VALUES(NULL, :name, :parent)");
    query.bindValue(":name", name);
    query.bindValue(":parent", parent_notebook);
    if(!query.exec())
        qDebug() << query.lastError();
    refresh();
    return 0; //TODO return record id
}

bool NotebooksModel::deleteNotebook(int id){
    if (id == 0)
        return false;
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("DELETE FROM notebooks WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    refresh();
    return true;
}

int NotebooksModel::addNotebook(QString parent_name, QString name){
    auto db = QSqlDatabase::database("localdatabase");
    QSqlQuery query(db);
    query.prepare("SELECT id FROM notebooks WHERE name = ?");
    query.addBindValue(parent_name);
    query.exec();
    if (query.next())
        return addNotebook(query.value(0).toInt(), name);
    else{
        qDebug() << "ERROR";
        qDebug() << query.lastError();
        qDebug() << sqlite::getLastExecutedQuery(query);
        return 0;
    }
}

int NotebooksModel::countNotes(int id){
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(id) FROM notes WHERE notebook_id = ?");
    query.addBindValue(id);
    query.exec();
    if (query.next())
        return query.value(0).toInt();
    else
        return 0;
}
