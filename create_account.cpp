#include "create_account.h"
#include "ui_create_account.h"
#include <QPushButton>

create_account::create_account(const QString &title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::create_account)
{
    ui->setupUi(this);

    this->setWindowTitle(title);

    this->setStyleSheet(
        "QDialog {"
        "   background-color: lavender;" // Светло-сиреневый фон
        "}"
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

    // Устанавливаем иконки на кнопки "Ок" и "Отмена"
    QIcon okIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsOK.png"); // Путь к изображению "Ок"
    QIcon cancelIcon("C:/Users/lizas/OneDrive/Desktop/course/image/iconsDelet.png"); // Путь к изображению "Отмена"

    // Получаем кнопки из QDialogButtonBox
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = ui->buttonBox->button(QDialogButtonBox::Cancel);

    // Устанавливаем иконки
    if (okButton) {
        okButton->setIcon(okIcon);
        okButton->setIconSize(QSize(32, 32)); // Размер иконки
    }

    if (cancelButton) {
        cancelButton->setIcon(cancelIcon);
        cancelButton->setIconSize(QSize(32, 32)); // Размер иконки
    }

    ui->changed_title->setText(title);
    ui->error->setStyleSheet("color: red;");
    ui->error->setVisible(false);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

create_account::~create_account()
{
    delete ui;
}

void create_account::SetErrorText(const QString &text)
{
    ui->error->setText(text);
}

void create_account::SetErrorVisible(bool status)
{
    ui->error->setVisible(status);
}

void create_account::SetError(const QString &text)
{
    SetErrorText(text);
    SetErrorVisible(true);
}

QString create_account::GetUsername()
{
    return ui->username_line->text();
}

int create_account::GetPassword()
{
    return ui->password_name->text().toInt();
}
