#ifndef CREATE_NOTE_H
#define CREATE_NOTE_H

#include <QDialog>

namespace Ui {
class create_note;
}

class create_note : public QDialog
{
    Q_OBJECT

public:
    explicit create_note(int curr_note = 0, QWidget *parent = nullptr);
    ~create_note();

    QString get_name() const;
    QString get_desc() const;
    int get_type() const;
    QDateTime get_time() const;

private:
    Ui::create_note *ui;
};

#endif // CREATE_NOTE_H
