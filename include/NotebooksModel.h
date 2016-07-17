#pragma once
#include <QSqlQueryModel>
#include <QDAte>
#include <QMutex>

class NotebooksModel : public QSqlQueryModel{
    Q_OBJECT
    Q_PROPERTY(int currentId MEMBER currentId)
   public:
       explicit NotebooksModel(QObject *parent, QSqlDatabase& db);
       void refresh();
       QVariant data(const QModelIndex &index, int role) const override;


       Q_INVOKABLE int addNotebook(int parent_id, QString name);
       Q_INVOKABLE int addNotebook(QString parent_name, QString name);

       Q_INVOKABLE void update(int id, QString name);
       Q_INVOKABLE int countNotes(int id);
//       Q_INVOKABLE int countNotes();
       Q_INVOKABLE bool doesDayHaveNote(int id, QDate date);
       Q_INVOKABLE bool deleteNotebook(int id);

   signals:

   public slots:

    protected:
       QHash<int, QByteArray> roleNames() const override;
   private:
       void prepareDatabase();
       const static char* COLUMN_NAMES[];
       const static char* SQL_SELECT;
        QHash<int, QByteArray> roles;
        int currentId;
        QSqlDatabase db;
        QString search;
};
