#include "note.h"
#include "ui_note.h"

note::note(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::note)
{
    ui->setupUi(this);
}

note::~note()
{
    delete ui;
}

void note::set_title(const QString& value)
{
    ui->title->setText(value);
}

void note::set_desc(const QString& value)
{
    ui->desc->setText(value);
}

void note::set_time(const QString& value)
{
    ui->time->setText(value);
}

const QString& note::get_title() const
{
    return ui->title->text();
}

const QString& note::get_desc() const
{
    return ui->desc->text();
}

const QString& note::get_time() const
{
    return ui->time->text();
}
