#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <handler.h>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

signals:
    void s_new_adr(QString, int, QString, int);

private slots:
    void on_pushButton_clicked();

public:
    explicit Dialog(Handler *, QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
