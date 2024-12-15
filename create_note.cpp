#include "create_note.h"
#include "ui_create_note.h"

create_note::create_note(int curr_note, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::create_note)
{
    ui->setupUi(this);

    this->setWindowTitle("Создать запись");

    this->setStyleSheet(
        "QDialog {"
        "   background-color: lavender;" // Светло-сиреневый фон
        "}"
        "QPushButton {"
        "   background-color: purple;" // Фиолетовый цвет фона
        "   color: white;"             // Белый цвет текста
        "   border-radius: 5px;"       // Закругленные углы
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #8A2BE2;"
        "}"
        );

    ui->type->setStyleSheet(
        "QComboBox {"
        "   background-color: purple;" // Светло-сиреневый фон
        "   color: black;"               // Черный текст
        "   border-radius: 5px;"         // Закругленные углы
        "   padding: 5px;"               // Отступы
        "}"
        "QComboBox:hover {"
        "   background-color: #8A2BE2;"  // Темно-сиреневый при наведении
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: lavender;" // Фон выпадающего списка
        "   color: black;"               // Черный текст
        "   border: 1px solid purple;"   // Граница
        "}"
        );

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    ui->date->setDateTime(QDateTime::currentDateTime());

    ui->type->setCurrentIndex(curr_note);
}

create_note::~create_note()
{
    delete ui;
}

QString create_note::get_name() const
{
    return ui->name->text();
}

QString create_note::get_desc() const
{
    return ui->desc->toPlainText();
}

int create_note::get_type() const
{
    return ui->type->currentIndex();
}

QDateTime create_note::get_time() const
{
    return ui->date->dateTime();
}

