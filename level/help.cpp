#include "help.h"
#include "ui_help.h"
#include <qwt_plot_marker.h>

Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
}

Help::~Help()
{
    delete ui;
}



