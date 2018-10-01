#include "plotclass.h"
#include "qwt_scale_widget.h"

plotClass::plotClass(QWidget *parent):
    QwtPlot(parent)
{
    ;
}

void plotClass::updateLayout()
{
    QwtPlot::updateLayout(); //WTF FEZ TODA A DIFERENÇA
    int width_yRight;
    int height_xBottom;
    int width_yLeft;
    int h;
    int w;
    if(this->axisEnabled(QwtPlot::yRight))
        width_yRight = this->axisWidget(QwtPlot::yRight)->width();
    else
        width_yRight = 0;
    if(this->axisEnabled(QwtPlot::yLeft) && this->axisEnabled(QwtPlot::xBottom))
    {
        height_xBottom = this->axisWidget(QwtPlot::xBottom)->height();
        width_yLeft = this->axisWidget(QwtPlot::yLeft)->width();
    }
    else
    {
        height_xBottom = 0;
        width_yLeft = 0;
    }
    h = this->height();
    w = this->width();
    if(h < w){
        h = this->height();
        w = (h-height_xBottom) + (width_yLeft+width_yRight);
    }
    else{
        w = this->width();
        h = height_xBottom + (w-(width_yLeft+width_yRight));
    }
    while(h > this->parentWidget()->height() || w > this->parentWidget()->width())
    {
        if(w > this->parentWidget()->width()){
            w = this->parentWidget()->width();
            h = height_xBottom + (w-(width_yLeft+width_yRight));
        }
        if(h > this->parentWidget()->height()){
            h = this->parentWidget()->height();
            w = (h-height_xBottom) + (width_yLeft+width_yRight);
        }
    }
    this->resize(w,h);
    this->move((this->parentWidget()->width()-w)/2, (this->parentWidget()->height()-h)/2);
}

