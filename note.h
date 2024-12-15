#ifndef NOTE_H
#define NOTE_H

#include <QWidget>

namespace Ui {
class note;
}

class note : public QWidget
{
    Q_OBJECT

public:
    explicit note(QWidget *parent = nullptr);
    ~note();

    void set_title(const QString& value);
    void set_desc(const QString& value);
    void set_time(const QString& value);

    const QString& get_title() const;
    const QString& get_desc() const;
    const QString& get_time() const;

private:
    Ui::note *ui;
};

#endif // NOTE_H
