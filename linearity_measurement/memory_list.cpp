#include "memory_list.h"
#include "ui_memory_list.h"

memory_list::memory_list(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::memory_list)
{
    ui->setupUi(this);

    QString addr = qApp->applicationDirPath() + QString("/ico");
    setWindowIcon(QIcon(addr+"/mm"));


    QStringList headers;
    headers << "Имя" << "Модель" << "Серия" << "Время" << "Дата" << "Кол-во измерений";

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

memory_list::~memory_list()
{
    delete ui;
}

void memory_list::setInfo(QVector<QStringList> info)
{
    int count =  ui->tableWidget->rowCount();
    if (count)
    {
        for (int i = 0; i < count; ++i)
        {
            ui->tableWidget->removeRow(0);
        }
    }


    for (int i = 0; i < info.count(); ++i)
    {
        ui->tableWidget->insertRow(i);
        for (int j = 0; j < 6; ++j)
        {
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(info[i][j]));
        }
    }
}

void memory_list::on_pushButton_clicked()
{
    QModelIndexList selectedRowst = ui->tableWidget->selectionModel()->selectedRows();
    QStringList result;

    for (int i = 0; i < selectedRowst.count(); ++i)
    {
        int selectedRow =  selectedRowst[i].row();
        QString name = ui->tableWidget->item(selectedRow,0)->text();
        result.append(name);
    }
    emit ready_to_upload(result);
}

void memory_list::on_pushButton_2_clicked()
{
    QModelIndexList selectedRowst = ui->tableWidget->selectionModel()->selectedRows();
    QStringList result;

    for (int i = 0; i < selectedRowst.count(); ++i)
    {
        int selectedRow =  selectedRowst[i].row();
        QString name = ui->tableWidget->item(selectedRow,0)->text();
        result.append(name);
    }
    emit delete_from_mm(result);

    for (int i = 0; i < selectedRowst.count(); ++i)
    {
        int selectedRow =  selectedRowst[i].row();
        ui->tableWidget->removeRow(selectedRow-i);
    }
}

void memory_list::on_deleteall_clicked()
{
    QStringList result;
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        QString name = ui->tableWidget->item(i,0)->text();
        result.append(name);
    }

    emit delete_from_mm(result);

    int count =  ui->tableWidget->rowCount();
    if (count)
    {
        for (int i = 0; i < count; ++i)
        {
            ui->tableWidget->removeRow(0);
        }
    }

}
