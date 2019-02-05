#include "models.h"
#include "ui_models.h"
#include <QString>

models::models(Handler *obj,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::models)
{
    handler = obj;
    ui->setupUi(this);
    connect(this,SIGNAL(sig_change_gen(QList<QString>)),handler,SLOT(slot_change_gen(QList<QString>)));
    connect(this,SIGNAL(sig_change_exp(QList<QString>)),handler,SLOT(slot_change_exp(QList<QString>)));
}

models::~models()
{
    delete ui;
}

void models::on_pushButton_clicked()
{
    QList<QString> change_gen;
    change_gen.append(QString::number(ui->spinBox1->value()));
    change_gen.append(ui->gen_1->text());
    change_gen.append(ui->gen_2->text());
    change_gen.append(ui->gen_3->text());
    emit sig_change_gen(change_gen);

    QList<QString> change_exp;
    change_exp.append(QString::number(ui->spinBox2->value()));
    change_exp.append(ui->exp_1->text());
    change_exp.append(ui->exp_2->text());
    change_exp.append(ui->exp_3->text());
    emit sig_change_exp(change_exp);
}
