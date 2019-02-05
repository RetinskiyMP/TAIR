#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(Handler *handler,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QString addr = qApp->applicationDirPath() + QString("/ico");
    setWindowIcon(QIcon(addr+"/ip"));

    connect(this,SIGNAL(s_new_adr(QString,int,QString,int)),handler,SLOT(slot_newAdr(QString,int,QString,int)));

    ui->adr1->setInputMask("d00.000.000.000");
    ui->adr2->setInputMask("d00.000.000.000");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    emit s_new_adr(ui->adr1->text(),ui->port1->value(),ui->adr2->text(),ui->port2->value());
    this->close();
    delete ui;
}
