#include "settingswin.h"
#include "ui_settingswin.h"
#include <QColorDialog>

SettingsWin::SettingsWin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWin),
    color(QColor(255,0,0))
{
    ui->setupUi(this);
    ui->lineEdit->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
    ui->editCCenter->setEnabled(false);
    ui->editInterval->setEnabled(false);
    ui->triangleFalse->setEnabled(false);
    ui->triangleTrue->setEnabled(false);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, color); // no transparency support
    ui->color->setPalette(pal);
}

SettingsWin::~SettingsWin()
{
    delete ui;
}

bool SettingsWin::get_checkBox_value()
{
    return ui->checkBox->checkState();
}

bool SettingsWin::get_checkBox_2_value()
{
    return ui->checkBox_2->checkState();
}

bool SettingsWin::get_checkBox_3_value()
{
    return ui->checkBox_3->checkState();
}

bool SettingsWin::get_checkBox_4_value()
{
    return ui->checkBox_4->checkState();
}

bool SettingsWin::get_radioButton_2_value()
{
    return ui->radioButton_2->isChecked();
}

void SettingsWin::on_radioButton_2_toggled(bool checked)
{
    ui->lineEdit->setEnabled(checked);
}

int SettingsWin::get_numberContour_value()
{
    if(this->get_contourConstant_option() != 4)
        return 1;
    if(get_radioButton_2_value())
        return ui->lineEdit->text().toInt();
    else
        return 10; // default
}

int SettingsWin::get_contourConstant_option()
{
    if(ui->radioButton_4->isChecked())
        return ui->comboBox_2->currentIndex(); // 0,1,2
    else if(ui->radioButton_5->isChecked())
        return 3;
    else
        return 4;
}

void SettingsWin::on_radioButton_4_toggled(bool checked)
{
    ui->comboBox_2->setEnabled(checked);
}

void SettingsWin::on_radioButton_5_toggled(bool checked)
{
    ui->lineEdit_2->setEnabled(checked);
}

double SettingsWin::get_lineEdit_2_value()
{
    return ui->lineEdit_2->text().toDouble();
}

void SettingsWin::on_colorSelect_clicked()
{
    QColorDialog dialog;
    color = dialog.getColor();

    QPalette pal = palette();
    pal.setColor(QPalette::Background, color); // no transparency support
    ui->color->setPalette(pal);

}

QColor SettingsWin::getColor()
{
    return color;
}

void SettingsWin::on_specifyCCenter_toggled(bool checked)
{
    ui->editCCenter->setEnabled(checked);
}

void SettingsWin::on_specifyInterval_toggled(bool checked)
{
    ui->editInterval->setEnabled(checked);
}

double SettingsWin::getCCenter()
{
    if(ui->defaultCCenter->isChecked())
        return -1.5;
    else
        return ui->editCCenter->text().toDouble();
}

double SettingsWin::getInterval()
{
    if(ui->defaultInterval->isChecked())
        return 0.4;
    else
        return ui->editInterval->text().toDouble();
}

void SettingsWin::on_checkBox_2_toggled(bool checked)
{
    ui->triangleFalse->setEnabled(checked);
    ui->triangleTrue->setEnabled(checked);
}

bool SettingsWin::getTriangleBool()
{
    return ui->triangleFalse->isChecked();
}

bool SettingsWin::get_checkBox_5_value()
{
    return ui->checkBox_5->checkState();
}

bool SettingsWin::get_checkBox_6_value()
{
    return ui->checkBox_6->checkState();
}
