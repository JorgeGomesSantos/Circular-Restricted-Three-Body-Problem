#include "print.h"
#include "ui_print.h"
#include <QFileDialog>
#include <QDebug>

Print::Print(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Print)
{
    ui->setupUi(this);
    fileName = QDir::currentPath().append("\\plotname.pdf"); // starting Empty file name
    ui->directoryEdit->setText(fileName);
    on_directoryEdit_editingFinished();
}

Print::~Print()
{
    delete ui;
}

QString Print::getFileName()
{
    return ui->directoryEdit->text();
}

bool Print::getBackground()
{
    return ui->backgroundCheck->checkState();
}

bool Print::getPdfText()
{
    return ui->pdfTextCheck->checkState();
}

double Print::getSize()
{
    return ui->sizeEdit->text().toDouble();
}

int Print::getResolution()
{
    return ui->resolutionEdit->text().toDouble();
}

void Print::on_directoryButton_clicked()
{
    fileName = QFileDialog::getSaveFileName(this,tr("Save File"), "plotname", tr("PDF Documents (*.pdf);;SVG Documents (*.svg);;Postscript Documents (*.ps);;Images(*.bmp *.cur *icns *.ico *.jpeg *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm)"));
    ui->directoryEdit->setText(fileName);
    on_directoryEdit_editingFinished();
}

void Print::on_directoryBox_currentIndexChanged(int index)
{
    if(fileName.isEmpty())
        return;
    QString ext;
    if(index == 0)
        ext = "";
    else
        ext = ui->directoryBox->currentText();
    QString ending = fileName.split(".").last();
    if(ending == fileName)
        fileName = fileName.append(ext);
    else{
        ending = QString(".").append(ending);
        int lastIndex = fileName.lastIndexOf(ending);
        fileName = fileName.replace(lastIndex, ending.size(), ext);
    }
    ui->directoryEdit->setText(fileName);
}

void Print::on_directoryEdit_editingFinished()
{
    fileName = ui->directoryEdit->text();
    QString ending = QString(".").append(fileName.split(".").last());
    if(ending == ".pdf")
        ui->directoryBox->setCurrentIndex(1);
    else if(ending == ".svg")
        ui->directoryBox->setCurrentIndex(2);
    else if(ending == ".ps")
        ui->directoryBox->setCurrentIndex(3);
    else if(ending == ".bmp")
        ui->directoryBox->setCurrentIndex(4);
    else if(ending == ".cur")
        ui->directoryBox->setCurrentIndex(5);
    else if(ending == ".icns")
        ui->directoryBox->setCurrentIndex(6);
    else if(ending == ".ico")
        ui->directoryBox->setCurrentIndex(7);
    else if(ending == ".jpeg")
        ui->directoryBox->setCurrentIndex(8);
    else if(ending == ".jpg")
        ui->directoryBox->setCurrentIndex(9);
    else if(ending == ".pbm")
        ui->directoryBox->setCurrentIndex(10);
    else if(ending == ".pgm")
        ui->directoryBox->setCurrentIndex(11);
    else if(ending == ".png")
        ui->directoryBox->setCurrentIndex(12);
    else if(ending == ".ppm")
        ui->directoryBox->setCurrentIndex(13);
    else if(ending == ".xbm")
        ui->directoryBox->setCurrentIndex(14);
    else if(ending == ".xpm")
        ui->directoryBox->setCurrentIndex(15);
    else
        ui->directoryBox->setCurrentIndex(0);
}
