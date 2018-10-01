#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_raster_data.h>
#include <qwt_plot_scaleitem.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_zoomer.h>
#include "help.h"
#include "print.h"
#include "plotclass.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setData();
    void setContourLevels();
    double setLagrangePoints(double startIteration);
    void setPoints();
    void polygonData();
    void setupCartesianAxis();
    void setupFirstLaunch();
    double get_functionResult(double c);
    void setupPlot();
    void setCenter();
    void setupColorAxis();
    void switchPoints();
    void setupStack();
    void moveToZoom();
    QPointF getUserQPointF();
    void setupZoomer();
    void setZoomText();
    void setMarkerAlignment();
    void setupParameters();
    void updateInfo(QwtPlotMarker *point);
    QString getString();
    bool fileExists(QString path);
    void indexedName();
    void setupGraphs();
    void setupAxis();
    void setColorGradient();
    void setDataGraphs();
    double yTransform(int value);
    int yInverse();
    QwtText HtmlLabel(QString textToHtml);

private slots:
    void on_actionHelp_triggered();

    void on_actionSave_triggered();

    void on_zoomIn_clicked();

    void on_zoomOut_clicked();

    void on_reset_clicked();

    void on_zoomPoints_activated(int index);

    void on_lineX_editingFinished();

    void on_lineY_editingFinished();

    void on_muBox_currentIndexChanged(int index);

    void on_muEdit_returnPressed();

    void on_muPush_clicked();

    void on_cCountEdit_editingFinished();

    void on_centerBox_currentIndexChanged(int index);

    void on_cCountBox_currentIndexChanged(int index);

    void on_centerEdit_editingFinished();

    void on_contourBox_currentIndexChanged(int index);

    void on_contourEdit_editingFinished();

    void on_lengthBox_currentIndexChanged(int index);

    void on_lengthEdit_editingFinished();

    void on_colorButton_clicked();

    void on_cmCheck_clicked(bool checked);

    void on_lagrangeCheck_clicked(bool checked);

    void on_primariesCheck_clicked(bool checked);

    void on_xyCheck_clicked(bool checked);

    void on_colorCheck_clicked(bool checked);

    void on_triangleCheck_clicked(bool checked);

    void on_linkCheck_clicked();

    void on_directoryButton_clicked();

    void on_fileNameEdit_editingFinished();

    void on_yPlaneEdit_editingFinished();

    void on_tabWidget_currentChanged(int index);

    void on_yPlaneSlider_valueChanged(int value);

    void on_yPlaneButton_clicked();

    void on_pushButton_clicked();

    void on_lagrangeBox_currentIndexChanged(int index);

    void on_bodyBox_currentIndexChanged(int index);

    void on_cEdit_editingFinished();

    void on_m1Edit_editingFinished();

    void on_m2Edit_editingFinished();

    void on_l1Edit_editingFinished();

    void on_l2Edit_editingFinished();

    void on_l3Edit_editingFinished();

    void on_l4Edit_editingFinished();

    void on_l5Edit_editingFinished();

    void controlZoomer();

    void on_directoryBox_currentIndexChanged(const QString &arg1);



private:
    Ui::MainWindow *ui;
    QwtPlotSpectrogram *curve;
    QwtPlotScaleItem *it[2];
    QwtPlotMarker *point;
    QwtPlotCurve *triangle;
    Help *helpWin;
    double cCenter, cCenterSave, cInterval, lengthSave;
    QColor colorPlot;
    int numberContour, cCountSave;
    QwtScaleWidget *rightAxis;
    double u;
    QwtPlotZoomer *zoomer, *zoomer2;
    QStack<QRectF> zoomStack;
    double xEdit, yEdit;
    double uEdit;
    double contourSave;
    plotClass *myPlot, *color, *macro;
    QString fileName;
    QwtPlotCurve *macroCurve;
    QwtPlotCurve *colorCurve;
    double y;
    QwtPlotMarker *centerLine;
    QwtPlotMarker *bottomLine;
    QwtPlotMarker *topLine;
    QwtPlotZoneItem *zoneColor;
    QwtPlotMarker *contourLine;
    QLinearGradient gradient;
    QString bodyLabel[3];
    QString lagrangeLabel[5];
};



#endif // MAINWINDOW_H
