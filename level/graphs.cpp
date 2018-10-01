#include "graphs.h"
#include "ui_graphs.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_layout.h>

Graphs::Graphs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graphs),
    macroCurve(new QwtPlotCurve()),
    colorCurve(new QwtPlotCurve()),
    y(0),
    cCenter(new QwtPlotMarker),
    bottomInterval(new QwtPlotMarker),
    topInterval(new QwtPlotMarker)
{
    ui->setupUi(this);
    contourLine = nullptr;
    setupLines();
    colorCurve->attach(ui->color);
    macroCurve->attach(ui->macro);
    ui->color->plotLayout()->setAlignCanvasToScales(true);
    ui->macro->plotLayout()->setAlignCanvasToScales(true);
    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
    gradient.setStart(0,0);
    gradient.setFinalStop(0,1);
    /*QwtPlotCanvas *canvasColor = new QwtPlotCanvas(ui->color);
    canvasColor->setPaintAttribute(QwtPlotCanvas::BackingStore,true);*/
}

Graphs::~Graphs()
{
    delete ui;
    delete []contourLine; // must delete to prevent leaks
}

void Graphs::setMacroData()
{
    int numPoints = 500;
    QVector<double> x(numPoints);
    QVector<double> z(numPoints);

    for(int i = 0; i <= (numPoints - 1); ++i)
    {
        x[i] = -1.5+i*3.0/(numPoints - 1);
        z[i] = -0.5*(x[i]*x[i]+y*y)-(1-u)/(qSqrt((x[i]-u)*(x[i]-u)+y*y))-u/(qSqrt((x[i]+1-u)*(x[i]+1-u)+y*y));
    }
    macroCurve->setSamples(x,z);
}

void Graphs::setColorData()
{
    int numPoints = 500;
    QVector<double> x(numPoints);
    QVector<double> z(numPoints);

    for(int i = 0; i <= numPoints - 1; ++ i)
    {
        x[i] = -1.5+i*3.0/(numPoints - 1);
        z[i] = -0.5*(x[i]*x[i]+y*y)-(1-u)/(qSqrt((x[i]-u)*(x[i]-u)+y*y))-u/(qSqrt((x[i]+1-u)*(x[i]+1-u)+y*y));
    }
    colorCurve->setSamples(x,z);

}

void Graphs::on_replotButton_clicked()
{
    setupY();
    setMacroData();
    setColorData();
    ui->macro->replot();
    ui->color->replot();
}

void Graphs::setupY()
{
    y = ui->yEdit->text().toDouble();
}

void Graphs::setupU(double u_external)
{
    u = u_external;
    setMacroData();
    setColorData();
    ui->macro->replot();
    ui->color->replot();
}

void Graphs::setupAxis(QwtInterval zInterval, int numberContour)
{
    double sizeInterval = zInterval.width(); // what if zInterval is given as positive values?!
    cCenter->setYValue((zInterval.maxValue()+zInterval.minValue())/2);
    bottomInterval->setYValue(zInterval.minValue());
    topInterval->setYValue(zInterval.maxValue());
    ui->macro->setAxisScale(QwtPlot::yLeft,zInterval.minValue()-sizeInterval*3,zInterval.maxValue()+sizeInterval*3);
    ui->color->setAxisScale(QwtPlot::yLeft,zInterval.minValue(),zInterval.maxValue());
    setColorLines(zInterval,numberContour);
    ui->color->replot();
    ui->color->updateAxes();
    ui->macro->replot();
    ui->macro->updateAxes();
}

void Graphs::setupLines()
{
    cCenter->setLineStyle(QwtPlotMarker::HLine);
    bottomInterval->setLineStyle(QwtPlotMarker::HLine);
    topInterval->setLineStyle(QwtPlotMarker::HLine);
    cCenter->attach(ui->macro);
    bottomInterval->attach(ui->macro);
    topInterval->attach(ui->macro);

    cCenter->setLabel(QwtText("CCenter"));
    cCenter->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    cCenter->setSpacing(0);
}

void Graphs::setColorLines(QwtInterval zInterval, int numberContour)
{
    if(contourLine != nullptr)
    {
        delete []contourLine;
    }
    contourLine = new QwtPlotMarker [numberContour];
    for(int i = 0; i < numberContour; ++i)
    {
        contourLine[i].attach(ui->color);
        contourLine[i].setLineStyle(QwtPlotMarker::HLine);
        contourLine[i].setYValue(zInterval.minValue()+(i+1)*zInterval.width()/(numberContour+1));
        contourLine[i].setLabel(QwtText(QString("%1").arg(contourLine[i].yValue())));
        contourLine[i].setLabelAlignment(Qt::AlignTop);
    }

}

void Graphs::setColorGradient(QColor userColor)
{
    int red,blue,green;
    red = userColor.red();
    blue = userColor.blue();
    green = userColor.green();
    gradient.setColorAt(0.0,QColor(red,green,blue,255));
    gradient.setColorAt(1.0,Qt::white);
    QBrush gradientBrush(gradient);
    ui->color->setCanvasBackground(gradientBrush);
}
