#ifndef PLOTCLASS_H
#define PLOTCLASS_H

#include <qwt_plot.h>

class plotClass: public QwtPlot
{
public:
    plotClass(QWidget *parent = NULL);
    void updateLayout();
};

#endif // PLOTCLASS_H
