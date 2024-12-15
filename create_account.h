#ifndef CREATE_ACCOUNT_H
#define CREATE_ACCOUNT_H

#include <QDialog>

namespace Ui {
class create_account;
}

class create_account : public QDialog
{
    Q_OBJECT

public:
    explicit create_account(const QString &title = "Создать аккаунт", QWidget *parent = nullptr);
    ~create_account();

    void SetErrorText(const QString &text);
    void SetErrorVisible(bool status);
    void SetError(const QString &text);

    QString GetUsername();
    int GetPassword();

private:
    Ui::create_account *ui;
};

#endif // CREATE_ACCOUNT_H
