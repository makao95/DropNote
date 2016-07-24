#include "Settings.h"
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <QPair>
#include <QDebug>
#include <QSqlError>
#include <QCryptographicHash>

Settings::Settings(QSqlDatabase *db):db(db), salt("714845"){
    prepareDatabase();
}

void Settings::prepareDatabase(){
    QSqlQuery query(*db);
    query.prepare("CREATE TABLE IF NOT EXISTS settings(id INTEGER PRIMARY KEY ASC, name VARCHAR(255), value TEXT)");
    query.exec();
    QVector< QPair <QString, QString> > defaults;
    defaults.push_back(qMakePair(QString("fontFamily"), QString("Verdana")));
    defaults.push_back(qMakePair(QString("fontSize"), QString("11")));
    defaults.push_back(qMakePair(QString("lastNoteId"), QString("0")));
    defaults.push_back(qMakePair(QString("theme"), QString("white")));
    defaults.push_back(qMakePair(QString("password"), QString("")));
    for (auto&& v : defaults){
        QSqlQuery selectQuery(*db);
        selectQuery.prepare("SELECT name FROM settings WHERE name = ?");
        selectQuery.addBindValue(v.first);
        selectQuery.exec();
        if (!selectQuery.next()){ // no value found
            QSqlQuery createQuery(*db);
            createQuery.prepare("INSERT INTO settings VALUES(NULL, ?, ?)");
            createQuery.addBindValue(v.first);
            createQuery.addBindValue(v.second);
            createQuery.exec();
        }
    }
}

QString Settings::getValueOf(QString name){
    QSqlQuery query(*db);
    query.prepare("SELECT name, value FROM settings WHERE name = ?");
    query.addBindValue(name);
    query.exec();
    if (query.next())
        return query.value(1).toString();
    else{
        qDebug() << "Couldn't find value of setting " << name;
        return QString("");
    }
}
#include "SqlLiteDatabase.h"

void Settings::setValue(QString name, QVariant value){
    QSqlQuery query(*db);
    query.prepare("UPDATE settings SET value=:value WHERE name=:name");
    query.bindValue(":value", value);
    query.bindValue(":name", name);
    if(!query.exec())
        qDebug() << query.lastError().text();
    qDebug() << sqlite::getLastExecutedQuery(query);

}

QString Settings::getFontFamily(){
    return getValueOf("fontFamily");
}

void Settings::setFontFamily(QString f){
    setValue("fontFamily", f);
}

void Settings::setFontSize(int s){
    setValue("fontSize", s);
}

int Settings::getFontSize(){
    return getValueOf("fontSize").toInt();
}

QString Settings::getTheme(){
    return getValueOf("theme");
}

void Settings::setTheme(QString t){
    setValue("theme", t);
}

bool Settings::checkPassword(QString p){
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(salt.toUtf8());
    hash.addData(p.toUtf8());
    if (QString(hash.result().toHex()) == getValueOf("password"))
        return true;
    else
        return false;
}

bool Settings::hasPassword(){
    if (getValueOf("password").length() > 0){
        if (checkPassword(""))
            return false;
        else
            return true;
    }else{
        return false;
    }
}

void Settings::setPassword(QString p){
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(salt.toUtf8());
    hash.addData(p.toUtf8());
    setValue("password",QString(hash.result().toHex()));
}
