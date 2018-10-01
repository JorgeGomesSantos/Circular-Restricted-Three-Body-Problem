#ifndef HELP_H
#define HELP_H

#include <QDialog>
#include <qwt_plot_marker.h>

namespace Ui {
class Help;
}

class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = 0);
    ~Help();

    void setValues(QwtPlotMarker *point[], QVector<int> positions);
    QString getString();
private:
    Ui::Help *ui;
};

#endif // HELP_H
