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
