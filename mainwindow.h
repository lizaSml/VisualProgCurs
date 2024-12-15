#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QFileDialog>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QTextStream>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDateEdit>
#include <QDebug>

#include <list>
#include "note.h"

#include <QTimer>
#include <QVector>
#include <QPointF>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define QAPP_DEFAULT_DB_NAME             ".app_accounts.db"
#define QAPP_DEFAULT_DB_TABLE_NAME       "user_accounts"
#define QAPP_DEFAULT_FIELD_LOGIN_NAME    "login"
#define QAPP_DEFAULT_FIELD_PASSWORD_NAME "password"
#define QAPP_DEFAULT_DB_TYPE             "QSQLITE"
#define QAPP_LAST_SESSION_FILENAME       ".app_last_session"
#define QAPP_DEFAULT_NOTE_DATA_SEP       '|'
#define QAPP_DEFAULT_TIME_FORMAT         "HH:mm dd.MM.yyyy"
#define QAPP_DEFAULT_WIN_TIMEOUT         20000

#define _debug() \
qDebug() << "<" << __func__  << ":" << __LINE__ << "> "

    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override; // Переопределяем paintEvent для анимации снежинок

private slots:
    void updateSnow(); // Метод для обновления снежинок

private:
    struct note_data
    {
        QString   name;
        QString   desc;
        QString   time;
        int       type;
        QDateTime qtime;

        note_data() { }

        note_data(const QString &name, const QString &desc, const QDateTime &qtime, int type)
        {
            this->name  = name;
            this->desc  = desc;
            this->time  = qtime.toString(QAPP_DEFAULT_TIME_FORMAT);
            this->type  = type;
            this->qtime = qtime;
        }

        void SaveToFile(QFile &file) const
        {
            QTextStream stream(&file);

            stream << name << QAPP_DEFAULT_NOTE_DATA_SEP <<
                desc << QAPP_DEFAULT_NOTE_DATA_SEP <<
                time << QAPP_DEFAULT_NOTE_DATA_SEP <<
                type << "\n";
        }

        bool LoadFromFile(QFile &file)
        {
            QByteArray data = file.readLine();

            QByteArrayList list = data.split(QAPP_DEFAULT_NOTE_DATA_SEP);

            if (list.length() != 4)
            {
                _debug() << "QByteArray length != 4 (" << list.length() << ")";
                return true;
            }

            name  = QString(list[0]);
            desc  = QString(list[1]);
            time  = QString(list[2]);
            type  = QString(list[3]).toInt();
            qtime = QDateTime::fromString(list[2], QAPP_DEFAULT_TIME_FORMAT);

            return false;
        }
    };

    Ui::MainWindow *ui;
    QSqlDatabase* base;
    std::list<std::pair<QListWidgetItem*, note_data>> note_list;

    QString currentUser;

    void DeleteNoteDialog(QListWidget* list);
    void SaveSession(QString filename);
    void LoadSession(QString filename);
    void MakeListItem(const note_data &data);
    void BuiltinFileHandle(void(MainWindow::*)(QString));
    void DataBasePrepare(QString name);
    void SaveNotesToDatabase(const QString &login);
    void LoadNotesFromDatabase(const QString &login);

    // Переменные для анимации снежинок
    QTimer *timer;
    QVector<QPointF> snowflakes; // Массив для хранения координат снежинок
    QVector<qreal> speeds;       // Массив для хранения скоростей снежинок

private slots:
    void CreateNoteDialog();
    void DeleteNoteWorkout();
    void DeleteNoteFood();

    void CreateAccount();
    void LoginAccount();
    void ExitAccount();

    void SaveToFile();
    void LoadFromFile();
};

#endif // MAINWINDOW_H
