#include <include/SqlLiteDatabase.h>
#include <iostream>
#include <ctime>
#include <QMapIterator>
#include <QVariant>


QString sqlite::getLastExecutedQuery(const QSqlQuery &query){
        QString str = query.lastQuery();
        QMapIterator<QString, QVariant> it(query.boundValues());

        it.toBack();

        while (it.hasPrevious())
        {
            it.previous();
            str.replace(it.key(),it.value().toString());
        }
        return str;
}

std::string sqlite::nowDateTime(){
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char dateTime[100];
    std::strftime(dateTime, sizeof(dateTime), "%F %T", std::localtime(&t));
    return std::string(dateTime);

}

SqlLiteException::SqlLiteException(std::string msg){
    this->msg = msg;
}

SqlLiteException::SqlLiteException(std::string msg, std::string query){
    this->msg = msg + query;
}

const char* SqlLiteException::what() const{
    return msg.c_str();
}


bool SqlLiteDatabase::open(std::string filename){
    if(sqlite3_open(filename.c_str(), &db) != SQLITE_OK)
        throw SqlLiteException("Can't open database" + std::string(sqlite3_errmsg(db)));
}

bool SqlLiteDatabase::query(std::string query){
    char* errorMessage = 0;
    if( sqlite3_exec(db, query.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK ){
        std::cout << "Error with query: " << query << " with message " <<errorMessage << std::endl;
        return false;
    }
    return true;
}

std::vector<SqlRow> SqlLiteDatabase::select(std::string query){
    sqlite3_stmt *statement;
    std::vector<SqlRow> results;

    if(sqlite3_prepare_v2(db, query.c_str(), -1, &statement, 0) == SQLITE_OK) {
        int cols = sqlite3_column_count(statement);
        int result = 0;
        while(true) {
            result = sqlite3_step(statement);

            if(result == SQLITE_ROW) {
                SqlRow row;
                for(int col = 0; col < cols; col++) {
                    row.push_back(sqlite3_column_name(statement, col), (char*)sqlite3_column_text(statement, col));
                }
                results.push_back(row);
            }
            else {
                break;
            }
        }
        sqlite3_finalize(statement);
    }else
        throw SqlLiteException(query);

    return results;

    std::vector<SqlRow> result;
    char* errorMessage = 0;
    auto callback = [result](void *data, int argc, char **argv, char **columnName){
        SqlRow row;
        for (int i=0; i < argc; i++){
            if (argv[i])
                row.push_back(std::string(columnName[i]), std::string(argv[i]));
            else
                row.push_back(std::string(columnName[i]), "NULL");
        }
    };

//    if (!sqlite3_exec(db, query.c_str(), &callback, 0, &errorMessage))
//        throw SqlLiteException("Something wrong with query: ", std::string(errorMessage));
}

std::string SqlRow::operator [](int index){
    auto it = row.begin();
    for (int i = 0; i < index; i++){
       it++;
    }
    if (it == row.end())
        throw SqlLiteException("out of bounds");
    return (it->second);
}

std::string SqlRow::operator [](std::string name){
    return row[name];
}

int SqlRow::getInt(std::string name){
    return std::stoi(this->row[name]);
}

void SqlRow::push_back(std::string name, std::string value){
   row[name] = value;
}
