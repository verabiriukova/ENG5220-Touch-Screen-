#ifndef WINDOW_H
#define WINDOW_H


#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

#include <QBoxLayout>
#include <QPushButton>

#include "adcreader.h"

// class definition 'Window'
class Window : public QWidget
{
	// must include the Q_OBJECT macro for for the Qt signals/slots framework to work with this class
	Q_OBJECT

public:
	Window(); // default constructor - called when a Window is declared without arguments

	~Window();

	void timerEvent( QTimerEvent * );

public slots:
	void clearPlot();
	void PEN1();
	void PEN2();
	void PEN3();

// internal variables for the window class
private:
	uint8_t pcount;
	QPushButton  *button;
    QPushButton  *pen1;
    QPushButton  *pen2;
	QwtPlot      *plot;
	QwtPlotCurve *curve;

	// layout elements from Qt itself http://qt-project.org/doc/qt-4.8/classes.html
	QVBoxLayout  *vLayout;  // vertical layout
	QHBoxLayout  *hLayout;  // horizontal layout

	static const int plotDataSize = 256;

	// data arrays for the plot
	double xData[plotDataSize];
	double yData[plotDataSize];

	double gain;
	int count;

	ADCreader *adcreader;
};

#endif // WINDOW_H
