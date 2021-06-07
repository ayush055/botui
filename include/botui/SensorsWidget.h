#ifndef _SENSORSWIDGET_H_
#define _SENSORSWIDGET_H_

#include <QQuickWidget>
#include "MenuBar.h"
#include "PlotWidget.h"

namespace Ui
{
	class SensorsWidget;
}

class SensorsWidget : public QQuickWidget
{
Q_OBJECT
public:
	SensorsWidget(Device *device, QQuickWidget *parent = 0);
	~SensorsWidget();
	
public slots:
	void update();
	
private:
	double rawValue(const int &i) const;
	double value(const int &i) const;
    
    QwtPlot *plot;
    QwtPlotCurve *curve1;
	
    int plotDataSize = 255; 
    double xData[plotDataSize];
    double yData[plotDataSize];
    
	MenuBar *m_menuBar;
	Ui::SensorsWidget *ui;
	//PlotHandle m_plots[2];
};

#endif
