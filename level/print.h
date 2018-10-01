#ifndef PRINT_H
#define PRINT_H

#include <QDialog>

namespace Ui {
class Print;
}

class Print : public QDialog
{
    Q_OBJECT

public:
    explicit Print(QWidget *parent = 0);
    ~Print();

    QString getFileName();
    bool getBackground();
    bool getPdfText();
    double getSize();
    int getResolution();
private slots:
    void on_directoryButton_clicked();

    void on_directoryBox_currentIndexChanged(int index);

    void on_directoryEdit_editingFinished();

private:
    Ui::Print *ui;
    QString fileName;
};

#endif // PRINT_H
