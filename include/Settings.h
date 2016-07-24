#pragma once
#include <QObject>
#include <QString>
#include <QSqlDatabase>

class Settings : public QObject{
    Q_OBJECT
public:
    Settings(QSqlDatabase* db);

    Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(int fontSize READ getFontSize WRITE setFontSize)
    Q_PROPERTY(QString theme READ getTheme WRITE setTheme)
    Q_PROPERTY(bool hasPassword READ hasPassword NOTIFY hasPasswordChanged)
//    Q_PROPERTY(int lastNoteId READ getLastNoteId WRITE setLastNoteId)

    QString getFontFamily();
    void setFontFamily(QString f);

    int getFontSize();
    void setFontSize(int s);

    QString getTheme();
    void setTheme(QString t);

    Q_INVOKABLE bool checkPassword(QString p);
    bool hasPassword();
    Q_INVOKABLE void setPassword(QString p);

signals:
    void fontFamilyChanged();
    void hasPasswordChanged();


//    int getLastNoteId();
//    void setLastNoteId();

private:
    QSqlDatabase* db;
    void prepareDatabase();
    QString getValueOf(QString name);
	void setValue(QString name, QVariant value);
    QString salt;
};
