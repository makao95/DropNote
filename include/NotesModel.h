#pragma once
#include <QSqlQueryModel>
#include <QDAte>
#include <QMutex>

class NotesModel : public QSqlQueryModel{
    Q_OBJECT
    Q_PROPERTY(int currentId MEMBER currentId)
    Q_PROPERTY(int currentNotebookId READ getCurrentNotebook WRITE setCurrentNotebook)
   public:
       explicit NotesModel(QObject *parent, QSqlDatabase& db);
       void refresh();
       virtual QVariant data(const QModelIndex &index, int role) const override;

       Q_INVOKABLE void update(int id, QString title, QString text);
       Q_INVOKABLE int addNote(int notebook_id, QString title, QString text);
       Q_INVOKABLE bool doesDayHaveNote(QDate date);
       Q_INVOKABLE bool deleteNote(int id);
       Q_INVOKABLE void setSearch(QString s);

       void setCurrentNotebook(int id);
       int getCurrentNotebook();
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
        int currentNotebook;
        QSqlDatabase db;
        QString search;
};

//const char* NotesModel::COLUMN_NAMES[] = {
//    "id",
//    "title",
//    "text",
//    "created",
//    "edited",
////    NULL
//};

//const char* NotesModel::SQL_SELECT =
//    "SELECT notes.id, notes.title, notes.text, notes.created, notes.edited"
//    " FROM notes";
////    " JOIN artists ON albums.artistid = artists.id";

