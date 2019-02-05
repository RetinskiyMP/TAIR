#ifndef MEMORY_LIST_H
#define MEMORY_LIST_H

#include <QDialog>
#include <QDebug>
#include <QVector>

namespace Ui {
class memory_list;
}

class memory_list : public QDialog
{
    Q_OBJECT

public:
    explicit memory_list(QWidget *parent = 0);
    void setInfo(QVector<QStringList>);
    ~memory_list();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_deleteall_clicked();

signals:
    void ready_to_upload(QStringList);
    void delete_from_mm(QStringList);

private:
    Ui::memory_list *ui;
};

#endif // MEMORY_LIST_H
