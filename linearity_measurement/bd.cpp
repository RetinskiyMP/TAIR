#include "bd.h"
#include "ui_bd.h"

#include <QPoint>
#include <QMessageBox>
#include <QFile>


BD::BD(QWidget *parent) :
    rows(0), QDialog(parent),
    ui(new Ui::BD)
{
    ui->setupUi(this);

    QString addr = qApp->applicationDirPath() + QString("/ico");
    setWindowIcon(QIcon(addr+"/bd"));

    QStringList headers;

    headers << "Модель"
            << "Старт" << "Стоп" << "Кол-во изм."
            << "Порт ген." << "Частота ген." << "Кол-во точек ген." << "Полоса ПЧ ген."
            << "Кол-во портов" << "Порт прем." << "Частота прием." << "Кол-во точек прием." << "Полоса ПЧ прием." << "Задержка";

    ui->tableWidget->setColumnCount(14);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    addr = qApp->applicationDirPath() + QString("/config.db3");

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(addr);
    db.open();


    QSqlQuery query(db);
    query.exec("SELECT * FROM models");

    for (int i = 0; query.next(); ++i)
    {
        rows++;
        ui->tableWidget->insertRow(i);
        for (int j = 0; j < 14; ++j)
        {
        ui->tableWidget->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        }
        ui->tableWidget->item(i,0)->setFlags(Qt::ItemIsSelectable);
    }

    ui->textBrowser->insertPlainText("✔ данные получены;\n");
}


BD::~BD()
{
    db.close();
    delete ui;
}

QStringList BD::getDataFromMM(QString id)
{
    QStringList result;

    QSqlQuery query(db);
    query.exec("SELECT * FROM memory WHERE name = '"+id+"';");

    for (int i = 0 ; query.next(); ++i)
    {
        for (int j = 0; j < 16; ++j)
        {
            result << query.value(j).toString();
        }

    }

    return result;
}

void BD::addMM(QStringList list)
{
    QString q = "INSERT INTO 'memory'('name','model','ser','time','date','points','Xvalues','Rgen','A','B','R1','R2','C','D','R3','R4')";
    q += " VALUES (";
    for (int i = 0; i < list.count()-1; ++i)
    {
        q += "'"+list[i]+"',";
    }
    q+="'"+list[list.count()-1]+"');";

    QSqlQuery query(db);
    query.exec(q);
}

void BD::slot_delete_memory(QString id)
{
    QSqlQuery query(db);
    query.exec("DELETE FROM memory WHERE name='"+id+"';");
}

QStringList BD::getMMnames()
{
    QStringList result;
    QSqlQuery query(db);
    query.exec("SELECT name FROM memory;");

    for (int i = 0 ; query.next(); ++i)
    {
        result << query.value(0).toString();
    }
    return result;
}

void BD::on_pushButton_4_clicked()
{
    int i = 0;

    QSqlQuery query(db);
    query.exec("SELECT * FROM models");

    for (i = 0 ; query.next(); ++i)
    {
        ui->tableWidget->insertRow(i);
        for (int j = 0; j < 14; ++j)
        {
        ui->tableWidget->setItem(i,j, new QTableWidgetItem(query.value(j).toString()));
        }
        ui->tableWidget->item(i,0)->setFlags(Qt::ItemIsSelectable);
    }


    if (ui->tableWidget->rowCount() > i)
    {
        int kol = ui->tableWidget->rowCount();
        while (i <= kol)
        {
            ui->tableWidget->removeRow(kol);
            kol--;
        }
    }

    ui->textBrowser->insertPlainText("✔ повторный запрос выполнен;\n");
}

void BD::on_pushButton_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}

void BD::on_pushButton_2_clicked()
{
    QMessageBox* pmbx = new QMessageBox("Подтверждение",
                        "Удалится вся выбранная строка. Продолжить?\n",
                        QMessageBox::Warning,
                        QMessageBox::Yes,
                        QMessageBox::No, QMessageBox::Escape);

    pmbx->setButtonText(QMessageBox::Yes,"Да");
    pmbx->setButtonText(QMessageBox::No, "Нет");

    int n = pmbx->exec();
    delete pmbx;

    int row = ui->tableWidget->selectionModel()->currentIndex().row();
    if (n == 3)
    {
        if (NULL != ui->tableWidget->item(row,0))
        {
            delete_data(ui->tableWidget->item(row,0)->text());
        }
        ui->tableWidget->removeRow(row);
    }
}

void BD::delete_data(QString id)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM models WHERE model='"+id+"';");

    if(query.isActive())
    {
       rows--;
       query.exec("DELETE FROM models WHERE model='"+id+"';");
    }

    if (query.lastError().text() == " ")
        ui->textBrowser->insertPlainText("✔ данные удалены;\n");
}

bool BD::checkModel(QString model)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM models WHERE model='"+model+"';");

    for (int i = 0; query.next(); ++i)
    {
        if (query.value(0).toString() == model) return true;
    }
    return false;
}

QStringList BD::getConfig(QString model)
{
    QSqlQuery query(db);
    query.exec("SELECT * FROM models WHERE model='"+model+"';");
    QStringList result;
    for (int i = 0; query.next(); ++i)
    {
        for (int j = 1; j < 14; ++j)
        {
            result << query.value(j).toString();
        }
    }
    return result;
}

void BD::update_data(QString id, QStringList data)
{
    QString q;
    q += "UPDATE models SET p_start='"+data[1]+"',";
    q += "p_stop='"+data[2]+"',";
    q += "p_count='"+data[3]+"',";
    q += "g_port='"+data[4]+"',";
    q += "g_freq_hz='"+data[5]+"',";
    q += "g_points='"+data[6]+"',";
    q += "g_ifbw_hz='"+data[7]+"',";
    q += "d_count_ports='"+data[8]+"',";
    q += "d_port='"+data[9]+"',";
    q += "d_freq_hz='"+data[10]+"',";
    q += "d_points='"+data[11]+"',";
    q += "d_ifbw_hz='"+data[12]+"',";
    q += "d_delay='"+data[13]+"' WHERE model='"+id+"';";

    QSqlQuery query(db);
    query.exec(q);

    if (query.lastError().text() == " ")
        ui->textBrowser->insertPlainText("✔ id["+id+"] обновлен;\n");
}

void BD::add_data(QStringList data)
{
    QString q;
    q += "INSERT INTO 'models'('model','p_start','p_stop','p_count',";
    q += "'g_port','g_freq_hz','g_points','g_ifbw_hz',";
    q += "'d_count_ports','d_port','d_freq_hz','d_points','d_ifbw_hz','d_delay') ";
    q += "VALUES (";
    for (int i = 0; i < 13; ++i)
    {
        q += "'"+data[i]+"',";
    }
    q += "'"+data[13]+"');";

    QSqlQuery query(db);
    query.exec(q);
    rows++;

    if (query.lastError().text() == " ")
        ui->textBrowser->insertPlainText("✔ id["+data[0]+"] добавлен в БД;\n");
}

void BD::on_accept_clicked()
{
    int row_count = ui->tableWidget->rowCount();
    QStringList data_of_row;
    QString id;

    if (checkRow())
    {
        for (int i = 0; i < rows; i++)
        {
            data_of_row.clear();
            id = ui->tableWidget->item(i,0)->text();
            for (int j = 0; j < 14; j++)
            {
                data_of_row << ui->tableWidget->item(i,j)->text();
            }
            update_data(id,data_of_row);
        }

        if (row_count > rows)
        {
            data_of_row.clear();
            for (int i = rows; i < row_count; i++)
            {
                ui->tableWidget->item(i,0)->setFlags(Qt::ItemIsSelectable);
                for (int j = 0; j < 14; j++)
                {
                    data_of_row << ui->tableWidget->item(i,j)->text();
                }
                add_data(data_of_row);
             }

         }
        emit d_update();
    }
    else
    {
        QMessageBox* pmbx = new QMessageBox("Ошибка",
                            "Есть незаполненные поля\n",
                                            QMessageBox::Warning,
                                            0,
                                            QMessageBox::Ok, QMessageBox::Escape);

        pmbx->exec();
        delete pmbx;
    }

}

bool BD::checkRow()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        for (int j = 0; j < 14; ++j)
        {
            if (ui->tableWidget->item(i,j) == NULL || ui->tableWidget->item(i,j)->text() == "") return false;
        }
    }
    return true;
}
