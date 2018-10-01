#ifndef GRAPHS_H
#define GRAPHS_H

#include <QDialog>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

namespace Ui {
class Graphs;
}

class Graphs : public QDialog
{
    Q_OBJECT

public:
    explicit Graphs(QWidget *parent = 0);
    ~Graphs();

    void setColorData();
    void setMacroData();
    void setupY();
    void setupU(double u_external);
    void setupAxis(QwtInterval zInterval, int numberContour);
    void setupLines();
    void setColorLines(QwtInterval zInterval, int numberContour);
    void setColorGradient(QColor userColor);
private slots:
    void on_replotButton_clicked();
private:
    Ui::Graphs *ui;
    QwtPlotCurve *macroCurve;
    QwtPlotCurve *colorCurve;
    double y;
    double u;
    QwtPlotMarker *cCenter;
    QwtPlotMarker *bottomInterval;
    QwtPlotMarker *topInterval;
    QwtPlotMarker *contourLine;
    QLinearGradient gradient;
};

#endif // GRAPHS_H
