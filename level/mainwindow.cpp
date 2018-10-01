// A maximização da janela conserva as dimensões iniciais COM FALHAS

// Create global u variable? **CORRIGIDO**
// Fazer delete do qwtplotmarker auxiliario **NÂO FAZER**
// Zoom num ponto quando u>0.5 **CORRIGIDO**
// Desativar zoom com rato **CORRIGIDO**
// Quando carrega no Replot e está zoomado **CORRIGIDO**
// zoom e mudar o ponto subitamente e zoom out **CORRIGIDO**

// mudança de M1 e M2 **CORRIGIDO**
// zoom com o rato **CORRIGIDO**
// zoom centrado **CORRIGIDO**
// texto no zoom ponto **CORRIGIDO**
// texto no valor u ponto **CORRIGIDO**
// espaço entre as coordenadas no help **CORRIGIDO**
// pdf que inclui txt **99% FUNCIONAL**
// opcao para incluir background **CORRIGIDO**
// tamanho dos labels dos markers **CORRIGIDO**
// posição dos labels dos markers **CORRIGIDO**
// indices utf-8 labels **CORRIGIDO**
// integrar o centro de massa dos dois corpos C **CORRIGIDO**
// italico labels **CORRIGIDO**
// print aquilo que está na tela apenas e não em opção **CORRIGIDO**
// mudar help para outro tipo de janela, CANCEL não faz sentido! **CORRIGIDO**
// botao RESET reinicializa o userdefined?
// maximize da janela **CORRIGIDO**
// funcao em que se especifica y noutra janela **CORRIGIDO**
// funcao com zoom e cores iguais à palete
// if body points are hidden don't need to ajust alignment
// remover actionprint **CORRIGIDO**
// manter aspect ratio quadrado **CORRIGIDO MAS INICIA FULL SCREEN**
// aumentar stack de zoom**CORRIGIDO**

// y color desta forma ou com cores interpoladas?
// QUAL o comportamento esperado de zoom com mouse e zoom com zoom in/zoom out?
// QUAL o comportamento esperado de zoom com o botão quando se usa o zoom do rato? DETETAR O ZOOM 0 DO ZOOM COM RATO?
// cor de background?
// o que fazer quando o intervalo é especificado negativamente?
// e quando os contornos forem negativos? (Deja vu mas poderei por uma janela de aviso!
// o mesmo para miu e coordenadas do ponto de zoom no textedit?
// o que fazer quando carrega no ponto? CARREGAR UMA VARIAVEL COM OS VALORES DE UTILIZADOR INICIALMENTE (0,0)!
// zoom com precisão 0.000000001
// zoom em C como opcao?
// Centro de massa faz-se pela definicao normal? Confirmacao?
// label massa por baixo?
// maximize da janela desta forma?
// resolucao e tamanho por defeito?
// fazer reset do zoom se começar a fazer zoom num ponto?
// desativar se começar a fazer zoom num ponto?
// guardar valores dos cortes no grafico window?
// comportamento do zoomer2 e zoomer em conjunto


/*
    RELATÓRIO

    ->Dificuldades encontradas
    ->Bibliotecas utilizadas sem grandes pormenores
    ->dificuldades nas cores transparencia
    ->Newton-Raphson função alterada com valor experimental 0.4

*/

/*
    CHANGELOG 28/01

    ->Tela com aspect ratio quadrado e fixo, inicializa em fullscreen;

*/

/*
 *  CHANGELOG UPDATED 17/09/2018
 *  ** DISPLAY OF MAINWINDOW
 *  ** IMPORTANT CORRECTIONS ON ZOOMING AND ENTER SLOT ADDED IN TEXT EDITS
 *  ** COMBOBOXES ADDED
 *  ** HIGHLIGHT WHEN LINEEDIT
 *  ** esconder o referencial do centro de massa retira o ponto do centro?
 * */

#include <QColorDialog>
#include <QFileInfo>
#include <QtCore>
#include <QtPrintSupport/QPrinter>
#include <QTextDocument>
#include <QFileDialog>
#include <QPrintDialog>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "help.h"
#include "print.h"

#include <qwt_color_map.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_rescaler.h>
#include <qwt_plot_scaleitem.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_zoomer.h>
#include <qwt_raster_data.h>
#include <qwt_scale_div.h>
#include <qwt_scale_widget.h>
#include <qwt_symbol.h>

class SpectrogramData: public QwtRasterData // cInterval deverá ser qAbs
{
    const double u;
public:
    SpectrogramData(double externalValue, double cCenter, double cInterval) :
        u(externalValue)
    {
        setInterval( Qt::XAxis, QwtInterval(-1.5, 1.5) ); // Por experiências em calculadora
        setInterval( Qt::YAxis, QwtInterval(-1.5, 1.5) ); // idem
        setInterval( Qt::ZAxis, QwtInterval(cCenter-cInterval/2, cCenter+cInterval/2) ); // idem
    }

    virtual double value( double x, double y ) const
    {
        const double v1 = -0.5*(x*x+y*y);
        const double v2 = -(1-u)/(qSqrt((x-u)*(x-u)+y*y));
        const double v3 = -u/(qSqrt((x+1-u)*(x+1-u)+y*y));
        return v1+v2+v3;
    }
};

class LinearColorMapRGB : public QwtLinearColorMap // barra das cores e palete aos utilizadores
{
    int colorR,colorG,colorB;
public:
    LinearColorMapRGB(QColor userColor, int numberContour)
    {
        colorR = userColor.red();
        colorG = userColor.green();
        colorB = userColor.blue();
        setColorInterval(QColor(colorR, colorG, colorB,0), QColor(colorR, colorG, colorB,255));
        setMode(QwtLinearColorMap::FixedColors); // Para não interpolar
        for(double i = 1; i <= numberContour ; i++)
            addColorStop(i/(numberContour + 1.0), QColor(colorR, colorG, colorB, i*255/numberContour));
    }
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    curve(new QwtPlotSpectrogram),
    it{new QwtPlotScaleItem(QwtScaleDraw::RightScale ,0.0),new QwtPlotScaleItem(QwtScaleDraw::BottomScale ,0.0)},
    point(new QwtPlotMarker [8]),
    triangle(new QwtPlotCurve),
    helpWin(new Help(this)),
    macroCurve(new QwtPlotCurve()),
    colorCurve(new QwtPlotCurve()),
    centerLine(new QwtPlotMarker),
    bottomLine(new QwtPlotMarker),
    topLine(new QwtPlotMarker),
    zoneColor(new QwtPlotZoneItem [3]),
    contourLine(nullptr)
{
    ui->setupUi(this);
    QHBoxLayout *layout = new QHBoxLayout(ui->plotWidget);
    QHBoxLayout *layoutMacro = new QHBoxLayout(ui->macroWidget);
    QHBoxLayout *layoutColor = new QHBoxLayout(ui->colorWidget);

    myPlot = new plotClass();
    macro = new plotClass();
    color = new plotClass();

    layout->addWidget(myPlot);
    layoutMacro->addWidget(macro);
    layoutColor->addWidget(color);

    setupFirstLaunch();
    showMaximized();

    connect(zoomer2, SIGNAL(zoomed(QRectF)), this, SLOT(controlZoomer()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete []contourLine;
    delete []zoneColor;
    delete []point;
}

void MainWindow::setupColorAxis()
{
    rightAxis = myPlot->axisWidget(QwtPlot::yRight);
    rightAxis->setColorBarEnabled(true);
    myPlot->enableAxis( QwtPlot::yRight );
    myPlot->plotLayout()->setAlignCanvasToScales(true);
    myPlot->axisWidget(QwtPlot::yRight)->setMargin(40);
}

void MainWindow::setupPlot()
{
    curve->setDisplayMode(QwtPlotSpectrogram::ContourMode, true);
    curve->attach(myPlot);
    triangle->attach(myPlot);
    triangle->hide();
    for(int j = 0; j < 8; ++j) // alterar!
    {
        if(j == 5 || j == 6) // massas em cime
            point[j].setLabelAlignment(Qt::AlignTop);
        else if(j == 7){
            point[j].setLabelAlignment(Qt::AlignBottom | Qt::AlignLeft); // funciona como massa!
            point[j].setValue(QPointF(0, 0));
        }
        else // lagrange por baixo
            point[j].setLabelAlignment(Qt::AlignBottom);
        point[j].setSpacing(0);
        point[j].setSymbol(new QwtSymbol(QwtSymbol::Ellipse, Qt::black, Qt::NoPen, QSize(4, 4)));
        point[j].attach(myPlot);
    }
    point[0].setLabel(HtmlLabel("L1"));
    point[1].setLabel(HtmlLabel("L2"));
    point[2].setLabel(HtmlLabel("L3"));
    point[3].setLabel(HtmlLabel("L4"));
    point[4].setLabel(HtmlLabel("L5"));
    point[5].setLabel(HtmlLabel("m1"));
    point[6].setLabel(HtmlLabel("m2"));
    point[7].setLabel(HtmlLabel("C"));
}

void MainWindow::setupParameters()
{
    // Color
    colorPlot= QColor(Qt::red);
    QString colorString = QString("background-color: rgb(%1, %2, %3)")
            .arg(colorPlot.red()).arg(colorPlot.green()).arg(colorPlot.blue());
    ui->colorButton->setStyleSheet(colorString.toStdString().c_str());
    ui->colorButton->update();

    // Mu settings
    ui->muEdit->setText(QString("0.01213"));
    u = ui->muEdit->text().toDouble();
    uEdit = u;
    xEdit = 0.0;
    yEdit = 0.0;


    // Number of Contours
    ui->cCountEdit->setText("10");
    numberContour = ui->cCountEdit->text().toInt();
    cCountSave = numberContour;

    // Center of Interval
    ui->centerEdit->setText("-1.5");
    cCenter = ui->centerEdit->text().toDouble();
    cCenterSave = cCenter;
    contourSave = cCenterSave;

    // Interval Length
    ui->lengthEdit->setText("0.4");
    cInterval = ui->lengthEdit->text().toDouble();
    lengthSave = cInterval;

    // Save Details
    fileName = QDir::currentPath().append("/file_name.pdf"); // starting Empty file name
    QFileInfo fi(fileName);
    ui->directoryEdit->setText(fi.absolutePath().append("/"));
    ui->fileNameEdit->setText(fi.completeBaseName());
    on_fileNameEdit_editingFinished();

    // Body Labels
    ui->cEdit->setText("C");
    ui->m1Edit->setText("m1");
    ui->m2Edit->setText("m2");
    bodyLabel[0] = ui->cEdit->text();
    bodyLabel[1] = ui->m1Edit->text();
    bodyLabel[2] = ui->m2Edit->text();

    // Lagrange Labels
    ui->l1Edit->setText("L1");
    ui->l2Edit->setText("L2");
    ui->l3Edit->setText("L3");
    ui->l4Edit->setText("L4");
    ui->l5Edit->setText("L5");
    lagrangeLabel[0] = ui->l1Edit->text();
    lagrangeLabel[1] = ui->l2Edit->text();
    lagrangeLabel[2] = ui->l3Edit->text();
    lagrangeLabel[3] = ui->l4Edit->text();
    lagrangeLabel[4] = ui->l5Edit->text();
}

void MainWindow::setupGraphs()
{
    // seting up the lines
    centerLine->setLineStyle(QwtPlotMarker::HLine);
    bottomLine->setLineStyle(QwtPlotMarker::HLine);
    topLine->setLineStyle(QwtPlotMarker::HLine);
    zoneColor[0].setOrientation(Qt::Horizontal);
    zoneColor[1].setOrientation(Qt::Horizontal);
    zoneColor[2].setOrientation(Qt::Horizontal);
    centerLine->attach(macro);
    bottomLine->attach(macro);
    topLine->attach(macro);
    zoneColor[0].attach(macro);
    zoneColor[1].attach(macro);
    zoneColor[2].attach(macro);

    centerLine->setLabel(QwtText("C Center"));
    centerLine->setLabelAlignment(Qt::AlignLeft | Qt::AlignTop);
    centerLine->setSpacing(0);

    colorCurve->attach(color);
    macroCurve->attach(macro);
    color->plotLayout()->setAlignCanvasToScales(true);
    macro->plotLayout()->setAlignCanvasToScales(true);
    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
    gradient.setStart(0,0);
    gradient.setFinalStop(0,1);
}

void MainWindow::setupCartesianAxis()
{
    QwtScaleDiv scaleDiv;
    it[0]->attach(myPlot);
    it[0]->setScaleDiv(scaleDiv);
    it[1]->attach(myPlot);
    it[1]->setScaleDiv(scaleDiv);
}
void MainWindow::setupZoomer()
{
    zoomer = new QwtPlotZoomer(myPlot->canvas());
    zoomer->setEnabled(false); //very niceeee
    setupStack();
    zoomer2 = new QwtPlotZoomer(myPlot->canvas());
    zoomer2->setTrackerMode(QwtPlotZoomer::AlwaysOn);
    zoomer2->setMousePattern(QwtEventPattern::MouseSelect2, Qt::MiddleButton);
    zoomer2->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);
    zoomer2->setRubberBandPen(QPen(QBrush(Qt::black), 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
}
void MainWindow::setupFirstLaunch()
{
    setupParameters();
    setupGraphs();
    setupPlot();
    setupCartesianAxis();
    setupZoomer();
    setupColorAxis();
    setData();
    setPoints(); // calculates and sets l1,l2,...
    polygonData(); // calculates the triangle vertices
    updateInfo(point); // remains to be seen
    setZoomText();
    moveToZoom();
    setMarkerAlignment();
    on_yPlaneSlider_valueChanged(0);
}

double MainWindow::setLagrangePoints(double startIteration)
{
    double c = startIteration, p=0; // current and previous
    double fPrevious, fdPrevious;
    do
    {
        p = c;
        fPrevious = p-u/((p-u+1)*qAbs(p-u+1))-(1-u)/((p-u)*qAbs(p-u));
        fdPrevious = 1+2*u/(qPow(p-u+1,2)*qAbs(p-u+1))+2*(1-u)/(qPow(p-u,2)*qAbs(p-u));
        c = p - fPrevious/fdPrevious;

    }while(qAbs(c-p)>0.00001);
    return c;
}

void MainWindow::setPoints()
{
    double l1,l2,l3;
    l1 = setLagrangePoints(-1);
    l2 = setLagrangePoints(-1 + 2 * u); // dado experimental
    l3 = setLagrangePoints(1);
    // Loads the points with the right data
    if(u <= 0.5){
        if(u == 0){
            point[0].setXValue(-1);
            point[1].hide();
        }
        else{
            point[0].setValue(QPointF(l1, 0));
            point[1].show();
        }
        point[2].setValue(QPointF(l3, 0));
    }
    else{
        if(u == 1){
            point[0].setXValue(1);
            point[1].hide();
        }
        else{
            point[0].setValue(QPointF(l3, 0));
            point[1].show();
        }
        point[2].setValue(QPointF(l1, 0));
    }
    point[1].setValue(QPointF(l2, 0));
    point[3].setValue(QPointF(u-0.5, qSqrt(3)/2));
    point[4].setValue(QPointF(u-0.5, -qSqrt(3)/2));
    point[5].setValue(QPointF(u, 0));
    point[6].setValue(QPointF(u-1, 0));
}

void MainWindow::polygonData() // Fills the polygon with data
{
    QVector<double> x(6);
    QVector<double> y(6);
    x[0] = point[5].xValue(); // M1
    x[1] = point[3].xValue(); // L4
    x[2] = point[6].xValue(); // M2
    x[3] = point[4].xValue(); // L5
    x[4] = point[5].xValue(); // M1
    y[0] = point[5].yValue(); // M1
    y[1] = point[3].yValue(); // L4
    y[2] = point[6].yValue(); // M2
    y[3] = point[4].yValue(); // L5
    y[4] = point[5].yValue(); // M1

    // Checks if the masses should be linked
    if(ui->linkCheck->isChecked()){
        x[5] = point[6].xValue();
        y[5] = point[6].yValue();
    }
    else{
        x[5] = point[5].xValue();
        y[5] = point[5].yValue();
    }
    triangle->setSamples(x,y);
}

double MainWindow::get_functionResult(double c)
{
    //y=0; talvez dê para usar a value function, a experimentar ************
    return -0.5*(c*c)-(1-u)/qSqrt((c-u)*(c-u))-u/qSqrt((c+1-u)*(c+1-u));
}

void MainWindow::on_actionHelp_triggered()
{
    helpWin->show();
}

void MainWindow::on_zoomIn_clicked() // resets all the stack
{
    zoomer->zoom(1);
    moveToZoom();
}

void MainWindow::on_zoomOut_clicked()
{
    zoomer->zoom(-1);
    moveToZoom();
}

void MainWindow::on_reset_clicked()
{
    zoomer->zoom(0); // go back home
    if(ui->zoomPoints->currentIndex() == 8){
        xEdit = 0;
        yEdit = 0;
        ui->lineX->setText(QString("0"));
        ui->lineY->setText(QString("0"));
    }
    moveToZoom();
}

void MainWindow::setupStack()
{
    zoomStack.push(QRectF(QPointF(-3, -3), QSizeF(6, 6)));
    zoomStack.push(QRectF(QPointF(-1.5, -1.5), QSizeF(3, 3))); //base
    for(int i = 1; i <= 100; i++)
        zoomStack.push(QRectF(QPointF(-1.5, -1.5), QSizeF(3 * qPow(0.5, i), 3 * qPow(0.5, i))));
    zoomer->setZoomStack(zoomStack, 1);
}

void MainWindow::moveToZoom()
{
    int select = ui->zoomPoints->currentIndex();
    QPointF optionPointCenter; //value
    if(zoomer->zoomRectIndex() == 0)
        zoomer->zoom(1);
    QRectF currentRectF = zoomer->zoomRect();
    switch(select){
    case 8:
        optionPointCenter = QPointF(xEdit, yEdit);
        break;
    default:
        optionPointCenter = point[select].value();
        break;
    }
    zoomer->moveTo(QPointF(optionPointCenter.x()-currentRectF.width() / 2, optionPointCenter.y()-currentRectF.height() / 2));
    zoomer2->setZoomStack(zoomer->zoomStack(), zoomer->zoomRectIndex());
    zoomer2->moveTo(QPointF(optionPointCenter.x()-currentRectF.width() / 2, optionPointCenter.y()-currentRectF.height() / 2));
}

void MainWindow::setZoomText()
{
    int select = ui->zoomPoints->currentIndex();
    switch(select){
    case 7:
        ui->lineX->setText(QString("0"));
        ui->lineY->setText(QString("0"));
        break;
    case 8:
        ui->lineX->setText(QString("%1").arg(xEdit));
        ui->lineY->setText(QString("%1").arg(yEdit));
        break;
    default:
        ui->lineX->setText(QString("%1").arg(point[select].xValue()));
        ui->lineY->setText(QString("%1").arg(point[select].yValue()));
        break;
    }
}

void MainWindow::setMarkerAlignment()
{
    if(u < 0.0004){
        if(u != 0)
            point[0].setLabelAlignment(Qt::AlignBottom | Qt::AlignLeft);
        else
            point[0].setLabelAlignment(Qt::AlignBottom);
        point[1].setLabelAlignment(Qt::AlignBottom | Qt::AlignRight);
    }
    else if(u > 0.40 && u < 0.54){
        point[7].setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);
    }
    else if(u > 0.9996){
        if(u != 1)
            point[0].setLabelAlignment(Qt::AlignBottom | Qt::AlignRight);
        else
            point[0].setLabelAlignment(Qt::AlignBottom);
        point[1].setLabelAlignment(Qt::AlignBottom | Qt::AlignLeft);
    }
    else{
        point[0].setLabelAlignment(Qt::AlignBottom);
        point[1].setLabelAlignment(Qt::AlignBottom);
        point[7].setLabelAlignment(Qt::AlignBottom | Qt::AlignLeft);
    }
}

void MainWindow::on_zoomPoints_activated(int index)
{
    if(index == 8){
        ui->lineX->setEnabled(true);
        ui->lineY->setEnabled(true);
    }
    else{
        ui->lineX->setEnabled(false);
        ui->lineY->setEnabled(false);
    }
    setZoomText();
    moveToZoom();
}

void MainWindow::on_lineX_editingFinished()
{
    xEdit = ui->lineX->text().toDouble();
    moveToZoom();
}

void MainWindow::on_lineY_editingFinished()
{
    yEdit = ui->lineY->text().toDouble();
    moveToZoom();
}

void MainWindow::on_muBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->muEdit->setEnabled(false);
        ui->muEdit->setText(QString("0.01213"));
        break;
    case 1:
        ui->muEdit->setEnabled(false);
        ui->muEdit->setText(QString("0.000003"));
        break;
    case 2:
        ui->muEdit->setEnabled(true);
        uEdit = ui->muEdit->text().toDouble();
        break;
    default:
        break;
    }
    on_muPush_clicked();
}

void MainWindow::on_muPush_clicked()
{
    u = ui->muEdit->text().toDouble();
    setPoints(); // calculates and sets l1,l2,...
    on_contourBox_currentIndexChanged(ui->contourBox->currentIndex());
    polygonData(); // calculates the triangle vertices
    updateInfo(point); // remains to be seen
    setZoomText();
    moveToZoom();
    setMarkerAlignment();
}

void MainWindow::on_muEdit_returnPressed()
{
    on_muPush_clicked();
}

void MainWindow::on_cCountBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->cCountEdit->setEnabled(false);
        ui->cCountEdit->setText("10");
        break;
    case 1:
        ui->cCountEdit->setEnabled(true);
        ui->cCountEdit->setText(QString("%1").arg(cCountSave));
        ui->cCountEdit->selectAll();
        ui->cCountEdit->setFocus();
        break;
    default:
        break;
    }
    setContourLevels();
}

void MainWindow::on_cCountEdit_editingFinished() // always going to be in the case 1, disabled otherwise!
{
    cCountSave = ui->cCountEdit->text().toInt();
    setContourLevels();
}

void MainWindow::setContourLevels() // Adds contour lines excluding limits and centered at the C value
{
    // Levels
    QList<double> contourLevels;
    double lim_inferior = curve->data()->interval(Qt::ZAxis).minValue();
    double length = curve->data()->interval(Qt::ZAxis).width();
    numberContour = ui->cCountEdit->text().toInt();
    for(int i = 1; i <= numberContour; i++)
        contourLevels.append(lim_inferior + i*length/(numberContour+1));
    curve->setContourLevels(contourLevels);

    // Color Levels
    curve->setColorMap(new LinearColorMapRGB(colorPlot, numberContour));

    // Color Axis
    QwtInterval zInterval = curve->data()->interval(Qt::ZAxis);
    rightAxis->setColorMap( zInterval, new LinearColorMapRGB(colorPlot,numberContour));
    myPlot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );

    // Replots
    myPlot->replot();
}

void MainWindow::on_contourBox_currentIndexChanged(int index)
{
    double zLevel;
    if(index == 4){
        ui->cCountBox->setEnabled(true);
        ui->centerBox->setEnabled(true);
        ui->contourEdit->setText("");
        ui->contourEdit->setEnabled(false);
        on_cCountBox_currentIndexChanged(ui->cCountBox->currentIndex());
        on_centerBox_currentIndexChanged(ui->centerBox->currentIndex());
    }
    else{
        ui->cCountBox->setEnabled(false);
        ui->centerBox->setEnabled(false);
        ui->cCountEdit->setEnabled(false);
        ui->centerEdit->setEnabled(false);
        if(index == 3){
            ui->contourEdit->setEnabled(true);
            ui->contourEdit->setText(QString("%1").arg(contourSave));
            ui->contourEdit->selectAll();
            ui->contourEdit->setFocus();
            zLevel = contourSave;
        }
        else{
            ui->contourEdit->setEnabled(false);
            if(index == 0)
                zLevel = get_functionResult(point[0].xValue());
            else if(index == 1)
                zLevel = get_functionResult(point[1].xValue());
            else if(index == 2)
                zLevel = get_functionResult(point[2].xValue());
            else
                return;
            ui->contourEdit->setText(QString("%1").arg(zLevel));
            qDebug() << zLevel;
        }
        ui->cCountEdit->setText("1");
        ui->centerEdit->setText(QString("%1").arg(zLevel));
        setData(); // updates the numberContour
    }
}

void MainWindow::on_contourEdit_editingFinished()
{
    contourSave = ui->contourEdit->text().toDouble();
    ui->centerEdit->setText(QString("%1").arg(contourSave));
    setData(); // updates the numberContour
}

void MainWindow::on_centerBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->centerEdit->setEnabled(false);
        ui->centerEdit->setText("-1.5");
        break;
    case 1:
        ui->centerEdit->setEnabled(true);
        ui->centerEdit->setText(QString("%1").arg(cCenterSave));
        ui->centerEdit->selectAll();
        ui->centerEdit->setFocus();
        break;
    default:
        return;
    }
    setData();
}

void MainWindow::on_centerEdit_editingFinished()
{
    cCenterSave = ui->centerEdit->text().toDouble();
    setData();
}

void MainWindow::setData()
{
    u = ui->muEdit->text().toDouble();
    cCenter = ui->centerEdit->text().toDouble();
    cInterval = ui->lengthEdit->text().toDouble();
    curve->setData(new SpectrogramData(u, cCenter, cInterval));
    setContourLevels();
}

void MainWindow::on_lengthBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->lengthEdit->setEnabled(false);
        ui->lengthEdit->setText("0.4");
        break;
    case 1:
        ui->lengthEdit->setEnabled(true);
        ui->lengthEdit->setText(QString("%1").arg(lengthSave));
        ui->lengthEdit->selectAll();
        ui->lengthEdit->setFocus();
        break;
    default:
        return;
    }
    setData();
}

void MainWindow::on_lengthEdit_editingFinished()
{
    lengthSave = ui->lengthEdit->text().toDouble();
    setData();
}

void MainWindow::on_colorButton_clicked()
{
    QColorDialog dialog;
    colorPlot = dialog.getColor();
    QString colorString = QString("background-color: rgb(%1, %2, %3)")
            .arg(colorPlot.red()).arg(colorPlot.green()).arg(colorPlot.blue());
    ui->colorButton->setStyleSheet(colorString.toStdString().c_str());
    ui->colorButton->update();
    setContourLevels();
}

void MainWindow::on_cmCheck_clicked(bool checked)
{
    if(checked){
        it[0]->show();
        it[1]->show();
    }
    else{
        it[0]->hide();
        it[1]->hide();
    }
    myPlot->replot();
}

void MainWindow::on_lagrangeCheck_clicked(bool checked)
{
    if(checked){
        point[0].show();
        point[1].show();
        point[2].show();
        point[3].show();
        point[4].show();
    }
    else{
        point[0].hide();
        point[1].hide();
        point[2].hide();
        point[3].hide();
        point[4].hide();
    }
    myPlot->replot();
}

void MainWindow::on_primariesCheck_clicked(bool checked)
{
    if(checked){
        point[5].show();
        point[6].show();
        point[7].show();
    }
    else{
        point[5].hide();
        point[6].hide();
        point[7].hide();
    }
    myPlot->replot();
}

void MainWindow::on_xyCheck_clicked(bool checked)
{
    if(checked){
        myPlot->enableAxis(QwtPlot::xBottom, true);
        myPlot->enableAxis(QwtPlot::yLeft, true);
    }
    else{
        myPlot->enableAxis(QwtPlot::xBottom, false);
        myPlot->enableAxis(QwtPlot::yLeft, false);
    }
}

void MainWindow::on_colorCheck_clicked(bool checked)
{
    if(checked)
         myPlot->enableAxis(QwtPlot::yRight,true);
    else
         myPlot->enableAxis(QwtPlot::yRight,false);
}

void MainWindow::on_triangleCheck_clicked(bool checked)
{
    if(checked){
        ui->linkCheck->setEnabled(true);
        polygonData();
        triangle->show();
    }
    else{
        ui->linkCheck->setEnabled(true);
        triangle->hide();
    }
    myPlot->replot();
}

void MainWindow::on_linkCheck_clicked()
{
    polygonData();
    myPlot->replot();
}

void MainWindow::updateInfo(QwtPlotMarker *point)
{
    ui->infoEdit->setText("The following points are referred to the CoM reference frame:");
    ui->infoEdit->append(QString("L1: (%1, 0)").arg(point[0].xValue()));
    ui->infoEdit->append(QString("L2: (%1, 0)").arg(point[1].xValue()));
    ui->infoEdit->append(QString("L3: (%1, 0)").arg(point[2].xValue()));
    ui->infoEdit->append(QString("L4: (%1, %2)").arg(point[3].xValue()).arg(point[3].yValue()));
    ui->infoEdit->append(QString("L5: (%1, %2)").arg(point[4].xValue()).arg(point[4].yValue()));
    ui->infoEdit->append(QString("m1: (%1, 0)").arg(point[5].xValue()));
    ui->infoEdit->append(QString("m2: (%1, 0)").arg(point[6].xValue()));
    ui->infoEdit->append(QString("C: (0, 0)"));
}

QString MainWindow::getString()
{
    return ui->infoEdit->toPlainText();
}

void MainWindow::on_actionSave_triggered()
{
    // set Focus to trigger editing finished in line edit
    ui->toolBar->setFocus();
    double stretch = ui->sizeEdit->text().toDouble() - myPlot->widthMM();

    // setup the qwt renderer
    QwtPlotRenderer renderer;
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasFrame, true);
    if(ui->backgroundCheck->checkState()){
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardBackground, true);
        renderer.setDiscardFlag(QwtPlotRenderer::DiscardCanvasBackground, true);
    }

    myPlot->replot(); // why? it works
    renderer.renderDocument(myPlot, fileName, QSizeF(myPlot->widthMM() + stretch, myPlot->heightMM() + stretch), 1200);

    // deals with text file
    if(ui->textCheck->checkState()){
        QString auxFile = fileName;
        QString ending = QString(".").append(auxFile.split(".").last());
        QFile file(auxFile.replace(auxFile.lastIndexOf(ending), ending.size(), ".txt"));
        QString pointCoordinates = getString();
        if (file.open(QFile::WriteOnly | QFile::Text)) // é write e não append
        {
            QTextStream stream(&file);
            stream << pointCoordinates << endl;
        }
    }    
    indexedName();
}

void MainWindow::on_directoryButton_clicked()
{
    // opens dialog text and gets directory
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QFileInfo(fileName).absolutePath(), QFileDialog::ShowDirsOnly);
    if(!filePath.isEmpty()){
        ui->directoryEdit->setText(filePath.append("/"));
        on_fileNameEdit_editingFinished();
    }
}

void MainWindow::on_fileNameEdit_editingFinished()
{
    if(!ui->fileNameEdit->text().isEmpty()){
        fileName = ui->directoryEdit->text()
                .append(ui->fileNameEdit->text())
                .append(ui->directoryBox->currentText());
        if(fileExists(fileName))
            indexedName();
    }
    else
        ui->fileNameEdit->setText(QFileInfo(fileName).completeBaseName());
}

void MainWindow::on_directoryBox_currentIndexChanged(const QString &arg1)
{
    QString ending = fileName.split(".").last();
    ending = QString(".").append(ending);
    fileName = fileName.replace(fileName.lastIndexOf(ending), ending.size(), arg1);
    if(fileExists(fileName))
        indexedName();
}

bool MainWindow::fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_file.exists() && check_file.isFile()) {
        return true;
    } else {
        return false;
    }
}

void MainWindow::indexedName()
{
    QRegExp rx("\\((\\d+)\\)");
    do // if I am in this block it already means I have a conflicting name
    {
        QFileInfo fi(fileName);
        QString oldName = fi.completeBaseName(); // only filename and not other dirs
        QString newName = oldName;
        int value = 0;
        int pos = 0, aux = 0;
        if(rx.indexIn(oldName) == -1)
            newName.append("(1)");
        else{
            while ((aux = rx.indexIn(oldName, pos)) != -1) {
                value = rx.cap(1).toInt();
                pos = aux + rx.matchedLength();
            }
            if(pos == oldName.size()){
                QString oldIndex = QString("%1").arg(value);
                newName.replace(newName.lastIndexOf(oldIndex), oldIndex.size(), QString("%1").arg(value + 1));
            }
            else
                newName.append("(1)");
        }
        fileName.replace(fileName.lastIndexOf(oldName), oldName.size(), newName);
    }while(fileExists(fileName));
    ui->fileNameEdit->setText(QFileInfo(fileName).completeBaseName());
}

void MainWindow::setupAxis()
{
    QwtInterval zInterval = curve->data()->interval(Qt::ZAxis);
    double sizeInterval = zInterval.width(); // what if zInterval is given as positive values?!
    centerLine->setYValue((zInterval.maxValue() + zInterval.minValue())/2);
    bottomLine->setYValue(zInterval.minValue());
    topLine->setYValue(zInterval.maxValue());
    macro->setAxisScale(QwtPlot::yLeft, zInterval.minValue() - sizeInterval, zInterval.maxValue() + sizeInterval);
    color->setAxisScale(QwtPlot::yLeft, zInterval.minValue(), zInterval.maxValue());
    zoneColor[0].setInterval(macro->axisInterval(Qt::YAxis).minValue(), zInterval.minValue());
    zoneColor[1].setInterval(zInterval.minValue(), zInterval.maxValue());
    zoneColor[2].setInterval(zInterval.maxValue(), macro->axisInterval(Qt::YAxis).maxValue());

    // color lines
    if(contourLine != nullptr){
        delete []contourLine;
    }
    contourLine = new QwtPlotMarker [numberContour];
    for(int i = 0; i < numberContour; ++i)
    {
        contourLine[i].attach(color);
        contourLine[i].setLineStyle(QwtPlotMarker::HLine);
        contourLine[i].setYValue(zInterval.minValue()+(i+1) * zInterval.width()/(numberContour+1));
        contourLine[i].setLabel(QwtText(QString("%1").arg(contourLine[i].yValue())));
        contourLine[i].setLabelAlignment(Qt::AlignTop);
        contourLine[i].setSpacing(zInterval.width()/(2*(numberContour+1)));
    }
    color->replot();
    color->updateAxes();
    macro->replot();
    macro->updateAxes();
}

void MainWindow::setColorGradient()
{
    gradient.setColorAt(0.0, QColor(colorPlot.red(), colorPlot.green(), colorPlot.blue(), 255));
    gradient.setColorAt(1.0, Qt::white);
    QBrush gradientBrush(gradient);
    color->setCanvasBackground(gradientBrush);
    zoneColor[0].setBrush(QBrush(Qt::white));
    zoneColor[1].setBrush(gradient);
    zoneColor[2].setBrush(QBrush(QColor(colorPlot.red(), colorPlot.green(), colorPlot.blue(), 255)));
    macro->setCanvasBackground(QBrush(Qt::white));
}

void MainWindow::setDataGraphs()
{
    int numPoints = 500;
    QVector<double> x(numPoints);
    QVector<double> z(numPoints);

    for(int i = 0; i <= numPoints - 1; ++ i)
    {
        x[i] = -1.5 + i * 3.0 / (numPoints - 1);
        z[i] = -0.5 * (x[i] * x[i] + y * y) - (1 - u) / (qSqrt((x[i] - u) * (x[i] - u) + y * y))- u / (qSqrt((x[i] + 1 - u) * (x[i] + 1 - u) + y * y));
    }
    colorCurve->setSamples(x,z);
    macroCurve->setSamples(x,z);
    macro->replot();
    color->replot();
}

void MainWindow::on_yPlaneEdit_editingFinished()
{
    double aux = ui->yPlaneEdit->text().toDouble();
    if(aux > yTransform(ui->yPlaneSlider->maximum()))
        aux = yTransform(ui->yPlaneSlider->maximum());
    else if(aux < yTransform(ui->yPlaneSlider->minimum()))
        aux = yTransform(ui->yPlaneSlider->minimum());

    y = aux;
    ui->yPlaneSlider->setValue(yInverse());
    ui->yPlaneEdit->setText(QString("%1").arg(aux));
    setDataGraphs();
}

void MainWindow::on_yPlaneSlider_valueChanged(int value)
{
    ui->yPlaneEdit->setText(QString::number(yTransform(value), 'g', 4));
    y = ui->yPlaneEdit->text().toDouble();
    setDataGraphs();
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    if(index == 3){
        setupAxis();
        setColorGradient();
        setDataGraphs();
    }
}

double MainWindow::yTransform(int value)
{
    QwtInterval yInterval = curve->data()->interval(Qt::YAxis);
    int range = ui->yPlaneSlider->maximum() - ui->yPlaneSlider->minimum();
    return yInterval.minValue() + value * yInterval.width() / range;
}

int MainWindow::yInverse()
{
    QwtInterval yInterval = curve->data()->interval(Qt::YAxis);
    int range = ui->yPlaneSlider->maximum() - ui->yPlaneSlider->minimum();
    return int((y - yInterval.minValue()) * range / yInterval.width());
}

void MainWindow::on_yPlaneButton_clicked()
{
    setDataGraphs();
}

void MainWindow::on_pushButton_clicked()
{
    on_actionSave_triggered();
}

void MainWindow::on_lagrangeBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->l1Edit->setEnabled(false);
        ui->l2Edit->setEnabled(false);
        ui->l3Edit->setEnabled(false);
        ui->l4Edit->setEnabled(false);
        ui->l5Edit->setEnabled(false);
        ui->l1Edit->setText("L1");
        ui->l2Edit->setText("L2");
        ui->l3Edit->setText("L3");
        ui->l4Edit->setText("L4");
        ui->l5Edit->setText("L5");
        point[0].setLabel(HtmlLabel(ui->l1Edit->text()));
        point[1].setLabel(HtmlLabel(ui->l2Edit->text()));
        point[2].setLabel(HtmlLabel(ui->l3Edit->text()));
        point[3].setLabel(HtmlLabel(ui->l4Edit->text()));
        point[4].setLabel(HtmlLabel(ui->l5Edit->text()));
        break;
    case 1:
        ui->l1Edit->setEnabled(true);
        ui->l2Edit->setEnabled(true);
        ui->l3Edit->setEnabled(true);
        ui->l4Edit->setEnabled(true);
        ui->l5Edit->setEnabled(true);
        ui->l1Edit->setText(lagrangeLabel[0]);
        ui->l2Edit->setText(lagrangeLabel[1]);
        ui->l3Edit->setText(lagrangeLabel[2]);
        ui->l4Edit->setText(lagrangeLabel[3]);
        ui->l5Edit->setText(lagrangeLabel[4]);
        point[0].setLabel(HtmlLabel(lagrangeLabel[0]));
        point[1].setLabel(HtmlLabel(lagrangeLabel[1]));
        point[2].setLabel(HtmlLabel(lagrangeLabel[2]));
        point[3].setLabel(HtmlLabel(lagrangeLabel[3]));
        point[4].setLabel(HtmlLabel(lagrangeLabel[4]));
        break;
    case 2:
        ui->l1Edit->setEnabled(false);
        ui->l2Edit->setEnabled(false);
        ui->l3Edit->setEnabled(false);
        ui->l4Edit->setEnabled(false);
        ui->l5Edit->setEnabled(false);
        ui->l1Edit->setText("");
        ui->l2Edit->setText("");
        ui->l3Edit->setText("");
        ui->l4Edit->setText("");
        ui->l5Edit->setText("");
        for(int i = 0; i < 5; ++i){
            point[i].setLabel(QwtText(""));
        }
        break;
    default:
        return;
    }
    myPlot->replot();
}

void MainWindow::on_bodyBox_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0:
        ui->m1Edit->setEnabled(false);
        ui->m2Edit->setEnabled(false);
        ui->cEdit->setEnabled(false);
        ui->m1Edit->setText("m1");
        ui->m2Edit->setText("m2");
        ui->cEdit->setText("C");
        point[5].setLabel(HtmlLabel(ui->m1Edit->text()));
        point[6].setLabel(HtmlLabel(ui->m2Edit->text()));
        point[7].setLabel(HtmlLabel(ui->cEdit->text()));
        break;
    case 1:
        ui->m1Edit->setEnabled(true);
        ui->m2Edit->setEnabled(true);
        ui->cEdit->setEnabled(true);
        ui->m1Edit->setText(bodyLabel[1]);
        ui->m2Edit->setText(bodyLabel[2]);
        ui->cEdit->setText(bodyLabel[0]);
        point[5].setLabel(HtmlLabel(bodyLabel[1]));
        point[6].setLabel(HtmlLabel(bodyLabel[2]));
        point[7].setLabel(HtmlLabel(bodyLabel[0]));
        break;
    case 2:
        ui->m1Edit->setEnabled(false);
        ui->m2Edit->setEnabled(false);
        ui->cEdit->setEnabled(false);
        ui->m1Edit->setText("");
        ui->m2Edit->setText("");
        ui->cEdit->setText("");
        point[5].setLabel(QwtText(""));
        point[6].setLabel(QwtText(""));
        point[7].setLabel(QwtText(""));
        break;
    default:
        return;
    }
    myPlot->replot();
}

QwtText MainWindow::HtmlLabel(QString textToHtml)
{
    QRegExp rxDigits("(\\d+)");
    QRegExp rxWords("(\\D+)");
    QString htmlString = "<i>";
    bool isStringFirst = true;
    int pos = 0;
    QStringList digitList;
    while ((pos = rxDigits.indexIn(textToHtml, pos)) != -1) {
        if(pos == 0)
            isStringFirst = false;
        digitList << rxDigits.cap(1);
        pos += rxDigits.matchedLength();
    }
    for(int i = 0; i < digitList.size(); ++i)
        digitList[i] = QString("<span style='font-size:12pt;'><sub>").append(digitList[i]).append("</sub></span>");

    QStringList wordList;
    pos = 0;
    while ((pos = rxWords.indexIn(textToHtml, pos)) != -1) {
        wordList << rxWords.cap(1);
        pos += rxWords.matchedLength();
    }
    for(int i = 0; i < wordList.size(); ++i)
        wordList[i] = QString("<span style='font-size:12pt;'>").append(wordList[i]).append("</span>");

    if(isStringFirst)
        for(int i = 0; i < wordList.size(); ++i){
            htmlString.append(wordList[i]);
            if(digitList.size())
                if(i <= (digitList.size() - 1))
                    htmlString.append(digitList[i]);
        }
    else
        for(int i = 0; i < digitList.size(); ++i){
            htmlString.append(digitList[i]);
            if(wordList.size())
                if(i <= (wordList.size() - 1))
                    htmlString.append(wordList[i]);
        }
    return QwtText(htmlString);
}

void MainWindow::on_cEdit_editingFinished()
{
    bodyLabel[0] = ui->cEdit->text();
    point[7].setLabel(HtmlLabel(bodyLabel[0]));
    myPlot->replot();
}

void MainWindow::on_m1Edit_editingFinished()
{
    bodyLabel[1] = ui->m1Edit->text();
    point[5].setLabel(HtmlLabel(bodyLabel[1]));
    myPlot->replot();
}

void MainWindow::on_m2Edit_editingFinished()
{
    bodyLabel[2] = ui->m2Edit->text();
    point[6].setLabel(HtmlLabel(bodyLabel[2]));
    myPlot->replot();
}

void MainWindow::on_l1Edit_editingFinished()
{
    lagrangeLabel[0] = ui->l1Edit->text();
    point[0].setLabel(HtmlLabel(lagrangeLabel[0]));
    myPlot->replot();
}

void MainWindow::on_l2Edit_editingFinished()
{
    lagrangeLabel[1] = ui->l2Edit->text();
    point[1].setLabel(HtmlLabel(lagrangeLabel[1]));
    myPlot->replot();
}

void MainWindow::on_l3Edit_editingFinished()
{
    lagrangeLabel[2] = ui->l3Edit->text();
    point[2].setLabel(HtmlLabel(lagrangeLabel[2]));
    myPlot->replot();
}

void MainWindow::on_l4Edit_editingFinished()
{
    lagrangeLabel[3] = ui->l4Edit->text();
    point[3].setLabel(HtmlLabel(lagrangeLabel[3]));
    myPlot->replot();
}

void MainWindow::on_l5Edit_editingFinished()
{
    lagrangeLabel[4] = ui->l5Edit->text();
    point[4].setLabel(HtmlLabel(lagrangeLabel[4]));
    myPlot->replot();
}

void MainWindow::controlZoomer()
{
    if(zoomer2->zoomRectIndex() == 0)
        zoomer2->zoom(1);
    if(zoomer2->zoomStack() == zoomer->zoomStack()){
        zoomer->setZoomStack(zoomer2->zoomStack(), zoomer2->zoomRectIndex());
    }
}
