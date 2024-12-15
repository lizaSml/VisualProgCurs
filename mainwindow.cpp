#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "create_note.h"
#include "create_account.h"

#include <QFileDialog>
#include <QTextStream>
#include <QTimer>

#include <iostream>
#include <functional>

#include <QRandomGenerator>
#include <QPainter> // Добавьте этот заголовочный файл
#include <QPainterPath>


void MainWindow::MakeListItem(const note_data &data)
{
    // создание виджета записи
    note* wgt = new note();

    wgt->set_title(data.name);
    wgt->set_desc(data.desc);
    wgt->set_time(data.time);

    QListWidget* list;

    switch (data.type)
    {
        case 0:
        {
            list = ui->workout_list;
            break;
        }
        case 1:
        {
            list = ui->food_list;
            break;
        }
    }
    // настройка виджета
    QDateTime current_time = QDateTime::currentDateTime();

    QListWidgetItem* item = new QListWidgetItem(list);
    QColor color = current_time < data.qtime ? QColor(Qt::GlobalColor::lightGray) : QColor("#FFB6C1");

    item->setBackground(color);
    item->setSizeHint(wgt->geometry().size());

    // добавление виджета в список
    list->setItemWidget(item, wgt);
    note_list.push_back(std::pair<QListWidgetItem*, note_data>(item, data));
}

void MainWindow::CreateNoteDialog()
{
    // создание окна создания записи
    create_note* dialog = new create_note(ui->tabWidget->currentIndex());

    QTimer::singleShot(QAPP_DEFAULT_WIN_TIMEOUT, dialog, SLOT(close()));

    dialog->show();
    // ожидание завершения работы пользователя
    if (dialog->exec() != QDialog::Accepted)
    {
        _debug() << "dialog not accepted, return";
        return;
    }

    note_data data(dialog->get_name(),
                   dialog->get_desc(),
                   dialog->get_time(),
                   dialog->get_type());
    // добавление записи в список
    MakeListItem(data);

    return;
}

void MainWindow::DeleteNoteDialog(QListWidget* list)
{
    QListWidgetItem* item = list->currentItem();

    if (item == nullptr)
    {
        _debug() << "ptr is null";
        return;
    }

    note_list.remove_if([item](const std::pair<QListWidgetItem*, note_data> &pair){ return pair.first == item; });

    list->removeItemWidget(item);
    delete item;
}

void MainWindow::DeleteNoteWorkout()
{
    DeleteNoteDialog(ui->workout_list);
}

void MainWindow::DeleteNoteFood()
{
    DeleteNoteDialog(ui->food_list);
}

void MainWindow::CreateAccount()
{
    // Сохраняем записи текущего пользователя перед созданием нового аккаунта
    if (!currentUser.isEmpty()) {
        SaveNotesToDatabase(currentUser);
        currentUser.clear();
    }

    // Очищаем интерфейс и список записей
    ui->workout_list->clear();
    ui->food_list->clear();
    note_list.clear();

    create_account* dialog = new create_account();

    dialog->show();

    if (dialog->exec() != QDialog::Accepted)
    {
        _debug() << "dialog not accepted, return";
        return;
    }

    if (base == nullptr)
    {
        _debug() << "databse not loaded, return";
        return;
    }

    QSqlQuery query(*base);

    QString entered_username = dialog->GetUsername();
    int entered_password     = dialog->GetPassword();

    if (entered_password == 0 ||
        entered_username == "")
    {
        _debug() << "incorrect login or password";
        return;
    }

    _debug() << "username: " << entered_username << ", password: " << entered_password;

    query.prepare("INSERT INTO " QAPP_DEFAULT_DB_TABLE_NAME "(" QAPP_DEFAULT_FIELD_LOGIN_NAME ", " QAPP_DEFAULT_FIELD_PASSWORD_NAME ")"
                  "VALUES (:username, :password);");

    query.bindValue(":username", entered_username);
    query.bindValue(":password", entered_password);

    query.exec();

    if (!query.isActive())
    {
        _debug() << "sql error: " << query.lastError();
        return;
    }

    ui->login_title->setText("Вы вошли как: " + entered_username);

    // Устанавливаем текущего пользователя
    currentUser = entered_username;
}

void MainWindow::LoginAccount()
{
    // создание диалогового окна
    create_account* dialog = new create_account("Войти в аккаунт");

    if (base == nullptr)
    {
        _debug() << "database not loaded";
        dialog->SetError("База данных не подключена");
    }

    dialog->show();

    if (dialog->exec() != QDialog::Accepted)
    {
        _debug() << "dialog not accepted, return";
        return;
    }

    if (base == nullptr)
    {
        _debug() << "database not loaded";
        return;
    }

    QSqlQuery query(*base);

    QString entered_username = dialog->GetUsername();
    int entered_password     = dialog->GetPassword();

    if (entered_password == 0 ||
        entered_username == "")
    {
        _debug() << "incorrect login or password";
        return;
    }

    _debug() << "username: " << entered_username << ", password: " << entered_password;

    query.prepare("SELECT * FROM " QAPP_DEFAULT_DB_TABLE_NAME " WHERE "
                  QAPP_DEFAULT_FIELD_LOGIN_NAME " = :username" );

    query.bindValue(":username", entered_username);

    query.exec();

    if (!query.first())
    {
        _debug() << "query.first() failed";
        return;
    }

    if (!query.isActive())
    {
        _debug() << "sql error: " << query.lastError();
        return;
    }

    QString database_username = query.value(0).toString();
    int database_password     = query.value(1).toInt();

    _debug() << "username: " << database_username << ", password: " << database_password;

    if (database_password != entered_password ||
        database_username != entered_username)
    {
        _debug() << "Incorrect login or password";
        return;
    }

    ui->login_title->setText("Вы вошли как: " + database_username);

    // Сохраняем записи предыдущего пользователя
    if (!currentUser.isEmpty()) {
        SaveNotesToDatabase(currentUser);
    }

    // Устанавливаем текущего пользователя
    currentUser = database_username;

    // Очищаем текущие записи
    ui->workout_list->clear();
    ui->food_list->clear();
    note_list.clear();

    // Загружаем записи пользователя из базы данных
    LoadNotesFromDatabase(currentUser);
}

void MainWindow::ExitAccount()
{
    if (!currentUser.isEmpty()) {
        // Сохраняем записи текущего пользователя в базу данных
        SaveNotesToDatabase(currentUser);

        // Удаляем записи текущего пользователя из базы данных
        QSqlQuery query(*base);
        query.prepare("DELETE FROM user_notes WHERE login = :login");
        query.bindValue(":login", currentUser);
        query.exec();

        currentUser.clear();
    }

    ui->login_title->setText("Вы не вошли в аккаунт");

    // Очищаем списки и данные
    ui->workout_list->clear();
    ui->food_list->clear();
    note_list.clear();
}

void MainWindow::SaveSession(QString filename)
{
    if (currentUser.isEmpty()) {
        _debug() << "No current user, cannot save session";
        return;
    }

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        _debug() << filename << " open failed";
        return;
    }

    for (auto item = note_list.cbegin(); item != note_list.cend(); item++)
    {
        item->second.SaveToFile(file);
    }
}

void MainWindow::LoadSession(QString filename)
{
    ui->workout_list->clear();
    ui->food_list->clear();
    note_list.clear();

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly))
    {
        _debug() << filename << " open failed";
        return;
    }

    while(!file.atEnd())
    {
        note_data data;

        if (data.LoadFromFile(file))
        {
            _debug() << "data.LoadFromFile failed";
            continue;
        }

        MakeListItem(data);
    }
}

void MainWindow::BuiltinFileHandle(void(MainWindow::*handler)(QString))
{
    QString filename = QFileDialog::getOpenFileName();

    if (filename.isEmpty())
    {
        _debug() << "filename is empty";
        return;
    }

    (this->*handler)(filename);
}

void MainWindow::SaveToFile()
{
    BuiltinFileHandle(&MainWindow::SaveSession);
}

void MainWindow::LoadFromFile()
{
    BuiltinFileHandle(&MainWindow::LoadSession);
}

void MainWindow::DataBasePrepare(QString name)
{
    base = new QSqlDatabase(QSqlDatabase::addDatabase(QAPP_DEFAULT_DB_TYPE));

    base->setDatabaseName(name);

    if (!base->open())
    {
        _debug() << "failed to open database";
        return;
    }

    QSqlQuery query(*base);

    // Создаем таблицу для пользователей
    query.exec("CREATE TABLE " QAPP_DEFAULT_DB_TABLE_NAME "("
               QAPP_DEFAULT_FIELD_LOGIN_NAME " TEXT PRIMARY KEY, "
               QAPP_DEFAULT_FIELD_PASSWORD_NAME " INT);");

    if (!query.isActive())
    {
        switch (query.lastError().nativeErrorCode().toInt())
        {
        case 1:
        {
            _debug() << "Database is exist, skip";
            break;
        }
        default:
        {
            _debug() << query.lastError().text();
            break;
        }
        }
    }

    // Создаем таблицу для записей
    query.exec("CREATE TABLE user_notes ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "login TEXT, "
               "name TEXT, "
               "desc TEXT, "
               "time TEXT, "
               "type INTEGER, "
               "FOREIGN KEY(login) REFERENCES " QAPP_DEFAULT_DB_TABLE_NAME "(" QAPP_DEFAULT_FIELD_LOGIN_NAME "));");

    if (!query.isActive())
    {
        switch (query.lastError().nativeErrorCode().toInt())
        {
        case 1:
        {
            _debug() << "Notes table is exist, skip";
            break;
        }
        default:
        {
            _debug() << query.lastError().text();
            break;
        }
        }
    }
}

void MainWindow::SaveNotesToDatabase(const QString &login)
{
    if (base == nullptr)
    {
        _debug() << "Database not loaded";
        return;
    }

    QSqlQuery query(*base);

    // Удаляем старые записи пользователя
    query.prepare("DELETE FROM user_notes WHERE login = :login");
    query.bindValue(":login", login);
    query.exec();

    // Добавляем новые записи пользователя
    for (const auto &item : note_list)
    {
        query.prepare("INSERT INTO user_notes (login, name, desc, time, type) "
                      "VALUES (:login, :name, :desc, :time, :type)");
        query.bindValue(":login", login);
        query.bindValue(":name", item.second.name);
        query.bindValue(":desc", item.second.desc);
        query.bindValue(":time", item.second.time);
        query.bindValue(":type", item.second.type);
        query.exec();
    }
}

void MainWindow::LoadNotesFromDatabase(const QString &login)
{
    if (base == nullptr)
    {
        _debug() << "Database not loaded";
        return;
    }

    QSqlQuery query(*base);

    query.prepare("SELECT name, desc, time, type FROM user_notes WHERE login = :login");
    query.bindValue(":login", login);
    query.exec();

    while (query.next())
    {
        note_data data;
        data.name = query.value("name").toString();
        data.desc = query.value("desc").toString();
        data.time = query.value("time").toString();
        data.type = query.value("type").toInt();
        data.qtime = QDateTime::fromString(data.time, QAPP_DEFAULT_TIME_FORMAT);

        MakeListItem(data);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Дневник тренировок и питания");

    this->setStyleSheet(
        "QMainWindow { background-color: lavender; }"
        "QPushButton {"
        "   background-color: purple;" // Фиолетовый цвет фона
        "   color: white;"             // Белый цвет текста
        "   border-radius: 5px;"       // Закругленные углы
        "   padding: 5px;"             // Отступы
        "}"
        "QPushButton:hover {"
        "   background-color: #8A2BE2;" // Темно-фиолетовый при наведении
        "}"
        );

    // Устанавливаем иконку для кнопки "Питание"
    QIcon foodIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsEDA.png"); // Путь к изображению
    ui->create_button_food->setIcon(foodIcon);
    ui->create_button_food->setIconSize(QSize(32, 32)); // Размер иконки

    // Устанавливаем иконку для кнопки "Тренировка"
    QIcon workoutIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsSport.png"); // Путь к изображению
    ui->create_button_workout->setIcon(workoutIcon);
    ui->create_button_workout->setIconSize(QSize(32, 32)); // Размер иконки

    // Устанавливаем иконку для кнопки "Удалить тренировку"
    QIcon deleteWorkoutIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsDelet.png"); // Путь к изображению
    ui->delete_button_workput->setIcon(deleteWorkoutIcon);
    ui->delete_button_workput->setIconSize(QSize(32, 32)); // Размер иконки

    // Устанавливаем иконку для кнопки "Удалить питание"
    QIcon deleteFoodIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsDelet.png"); // Путь к изображению
    ui->delete_button_food->setIcon(deleteFoodIcon);
    ui->delete_button_food->setIconSize(QSize(32, 32)); // Размер иконки


    // Инициализация таймера
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateSnow);
    timer->start(30); // Обновление каждые 30 мс

    // Создаём снежинки
    int snowflakeCount = 100; // Количество снежинок
    for (int i = 0; i < snowflakeCount; ++i) {
        qreal x = QRandomGenerator::global()->bounded(width());
        qreal y = QRandomGenerator::global()->bounded(height());
        snowflakes.append(QPointF(x, y));

        // Случайная скорость падения
        speeds.append(QRandomGenerator::global()->bounded(1, 3));
    }



    ui->tabWidget->setCurrentIndex(0);
    // загрузка последней сессии
    LoadSession(QAPP_LAST_SESSION_FILENAME);
    // загрузка базы данных
    DataBasePrepare(QAPP_DEFAULT_DB_NAME);
    ExitAccount();
    // подключение кнопок
    connect(ui->create_button_workout, SIGNAL(clicked()), this, SLOT(CreateNoteDialog()));
    connect(ui->create_button_food,    SIGNAL(clicked()), this, SLOT(CreateNoteDialog()));
    connect(ui->delete_button_workput, SIGNAL(clicked()), this, SLOT(DeleteNoteWorkout()));
    connect(ui->delete_button_food,    SIGNAL(clicked()), this, SLOT(DeleteNoteFood()));

    connect(ui->account_create, SIGNAL(triggered()), this, SLOT(CreateAccount()));
    connect(ui->account_login,  SIGNAL(triggered()), this, SLOT(LoginAccount()));
    connect(ui->account_exit,   SIGNAL(triggered()), this, SLOT(ExitAccount()));
    connect(ui->save,           SIGNAL(triggered()), this, SLOT(SaveToFile()));
    connect(ui->load,           SIGNAL(triggered()), this, SLOT(LoadFromFile()));
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Устанавливаем светло-голубой цвет для заливки и контура
    QColor lightBlue(173, 216, 230); // RGB для светло-голубого цвета
    painter.setPen(QPen(lightBlue, 1)); // Контур светло-голубой, тонкий
    painter.setBrush(lightBlue); // Заливка светло-голубой

    // Рисуем снежинки (кружочки)
    for (int i = 0; i < snowflakes.size(); ++i) {
        QPointF center = snowflakes[i];
        qreal radius = 3; // Радиус кружочка

        // Рисуем круг с центром в точке center и радиусом radius
        painter.drawEllipse(center, radius, radius);
    }
}

void MainWindow::updateSnow()
{
    for (int i = 0; i < snowflakes.size(); ++i) {
        // Обновляем позицию снежинки
        snowflakes[i].setY(snowflakes[i].y() + speeds[i]);
        snowflakes[i].setX(snowflakes[i].x() + QRandomGenerator::global()->bounded(-1, 2)); // Случайный сдвиг по X

        // Если снежинка упала за пределы экрана, возвращаем её вверх
        if (snowflakes[i].y() > height()) {
            snowflakes[i].setY(0);
            snowflakes[i].setX(QRandomGenerator::global()->bounded(width()));
        }
    }

    // Перерисовываем окно
    update();
}

MainWindow::~MainWindow()
{
    if (!currentUser.isEmpty()) {
        SaveNotesToDatabase(currentUser);
    }

    delete ui;
}
