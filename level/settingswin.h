#ifndef SETTINGSWIN_H
#define SETTINGSWIN_H

#include <QDialog>

namespace Ui {
class SettingsWin;
}

class SettingsWin : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWin(QWidget *parent = 0);
    ~SettingsWin();

    bool get_checkBox_value();
    bool get_checkBox_2_value();
    bool get_checkBox_4_value();
    bool get_checkBox_3_value();
    bool get_radioButton_2_value();
    int get_numberContour_value();
    int get_contourConstant_option();
    double get_lineEdit_2_value();
    QColor getColor();
    double getCCenter();
    double getInterval();
    bool getTriangleBool();
    bool get_checkBox_5_value();
    bool get_checkBox_6_value();
private slots:
    void on_radioButton_2_toggled(bool checked);

    void on_radioButton_4_toggled(bool checked);

    void on_radioButton_5_toggled(bool checked);

    void on_colorSelect_clicked();

    void on_specifyCCenter_toggled(bool checked);

    void on_specifyInterval_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

private:
    Ui::SettingsWin *ui;
    QColor color;
};

#endif // SETTINGSWIN_H
