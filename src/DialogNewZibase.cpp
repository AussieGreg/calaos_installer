#include "DialogNewZibase.h"
#include "ui_DialogNewZibase.h"
#include "formrules.h"

DialogNewZibase::DialogNewZibase(Room *r, int it, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewZibase),
    room(r),
    item(it)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->label_error_empty->hide();

    if(item == ITEM_TEMP)
    {
        ui->comboBox->hide();
         ui->label_5->hide();
    }
    else if (item == ITEM_ANALOG)
    {
        ui->comboBox->addItem(tr("Energy monitor sensor"), QString("energy"));
        //ui->comboBox->addItem(tr("Temperature sensor"), QString("temp"));
        ui->comboBox->addItem(tr("lux sensor"), QString("lux"));
        ui->comboBox->addItem(tr("total rain sensor"), QString("t_rain"));
        ui->comboBox->addItem(tr("wind sensor"), QString("wind"));
    }
    else if (item == ITEM_INPUT_SWITCH)
    {
        ui->comboBox->addItem(tr("Door/window sensor"), QString("detect"));
        ui->comboBox->addItem(tr("interuptor/remote"), QString("inter"));

    }

}

DialogNewZibase::~DialogNewZibase()
{
    delete ui;
}

void DialogNewZibase::on_buttonBox_accepted()
{
    if (ui->edit_name->text().isEmpty() ||
        ui->edit_host->text().isEmpty() ||
        ui->edit_id->text().isEmpty())
    {
        ui->label_error_empty->show();
        return;
    }

    Params p;

    p.Add("name", ui->edit_name->text().toUtf8().constData());
    p.Add("host", ui->edit_host->text().toUtf8().constData());
    p.Add("zibase_id", ui->edit_id->text().toUtf8().constData());
    p.Add("port", "17100");
    p.Add("zibase_sensor", ui->comboBox->itemData(ui->comboBox->currentIndex()).toString().toUtf8().constData());

    if (item == ITEM_TEMP)
    {   p.Add("type", "ZibaseTemp");
        p.Add("zibase_sensor", "temp");
    }else if (item == ITEM_ANALOG)
        p.Add("type", "ZibaseAnalogIn");
    else if (item == ITEM_INPUT_SWITCH)
    {
        p.Add("type", "ZibaseDigitalIn");
        p.Add("zibase_id2", ui->edit_id2->text().toUtf8().constData());
    }
    else
        reject();

    io = ListeRoom::Instance().createInput(p, room);

    accept();
}
