#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "sqlite3.h"
#include <QString>
#include <QSqlQuery>

namespace sqlite{
std::string nowDateTime();

QString getLastExecutedQuery(const QSqlQuery& query);

}

class SqlLiteException : std::exception {
public:
    SqlLiteException(std::string msg);
    SqlLiteException(std::string msg, std::string query);
    virtual const char* what() const;
private:
    std::string msg;
};

class SqlRow{
public:
    std::string operator[](std::string);
    std::string operator[](int);

    std::string getString(int index);
    std::string getString(std::string name);
    int getInt(int index);
    int getInt(std::string name);
    float getFloat(int index);
    float getFloat(std::string name);

    void push_back(std::string name, std::string value);
private:
    std::unordered_map<std::string, std::string> row;

};

class SqlLiteDatabase{
public:
    bool open(std::string filename);
    bool query(std::string query);
    std::vector<SqlRow> select(std::string query);
private:
    sqlite3* db;
};

