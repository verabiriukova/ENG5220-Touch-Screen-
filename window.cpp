#include "window.h"
#include "adcreader.h"


Window::Window() : gain(5), count(0)
{
button = new QPushButton("Clear",this);

connect(button, SIGNAL(clicked()), SLOT(clearPlot()));
   //Everytime the button is clicked the the clearPlot function is called

pen1 = new QPushButton("Blue", this);
connect(pen1, SIGNAL(clicked()), SLOT(PEN1()));
pen2 = new QPushButton("Red",this);
connect(pen2, SIGNAL(clicked()), SLOT(PEN2()));

QPushButton *pen3 = new QPushButton("Green",this);
connect(pen3, SIGNAL(clicked()), SLOT(PEN3()));

	pcount = 0;

	curve = new QwtPlotCurve;
	plot = new QwtPlot;

	curve->attach(plot);
	plot->setAxisScale(QwtPlot::xBottom,0.0,4500.0,0.0);		//set x scale
	plot->setAxisScale(QwtPlot::yLeft,0.0,4500.0,0.0);	//set y scale
	plot->setAxisAutoScale(QwtPlot::yRight, false);
	plot->setAxisAutoScale(QwtPlot::xTop, false);

	plot->show();


	vLayout = new QVBoxLayout;
	vLayout->addWidget(button);
	vLayout->addWidget(pen1);
	vLayout->addWidget(pen2);
	vLayout->addWidget(pen3);

    // plot to the left, horizontal layout
	hLayout = new QHBoxLayout;
	hLayout->addLayout(vLayout);
	hLayout->addWidget(plot);

	setLayout(hLayout);

	// This is a demo for a thread which can be
	// used to read from the ADC asynchronously.
	adcreader = new ADCreader();
	adcreader->start();
}

Window::~Window() {
	// tells the thread to no longer run its endless loop
	adcreader->quit();
	// wait until the run method has terminated
	adcreader->wait();
	delete adcreader;
}

void Window::timerEvent( QTimerEvent * )
{
	//get new data to plot
	struct coordinates rxData;
	while(adcreader->isEmpty()== false){
	adcreader->bufferRead(&rxData);

	// add the new input to the plot
	if(pcount<255){
	yData[pcount] = rxData.y;
	xData[pcount] = rxData.x;
	curve->setSamples(xData, yData, pcount);
	plot->replot();
	pcount++;
	}
	else{
		yData[pcount] = rxData.y;
		xData[pcount] = rxData.x;
		for(int i = 0; i<pcount; i++){
			yData[i] = yData[i+1];
			xData[i] = xData[i+1];
		}
	
	curve->setSamples(xData, yData, pcount-1);
	plot->replot();
	}

	
	fprintf(stderr,"x = %d, y = %d\n",rxData.x, rxData.y);
	}
}


void Window::clearPlot()
{
	
	for(int i = 0; i<plotDataSize; i++){
			yData[i] = NULL;
			xData[i] = NULL;
		}
	curve->setSamples(xData, yData, plotDataSize-1);

	pcount = 0;
	plot->replot();
}

void Window::PEN1()
{
    curve->setPen(* new QPen(Qt::blue,1,Qt::SolidLine));
	plot->replot();
}

void Window::PEN2()
{
    curve->setPen(*new QPen(Qt::red,3,Qt::DashLine));
	plot->replot();
}

void Window::PEN3()
{
    curve->setPen(*new QPen(Qt::green,5,Qt::DotLine));
	plot->replot();
}
