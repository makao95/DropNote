#pragma once
#include <string>
#include <QString>
#include <QSqlQuery>

namespace sqlite{
QString getLastExecutedQuery(const QSqlQuery& query);
}
