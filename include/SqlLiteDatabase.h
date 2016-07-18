#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "sqlite3.h"
#include <QString>
#include <QSqlQuery>

namespace sqlite{
QString getLastExecutedQuery(const QSqlQuery& query);
}
