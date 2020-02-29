/****************************************************************************
*
* main.cpp
*
* Purpose: main of moveIt project.
*
* Author: Nihanth
*
* Last update date: 19-06-2010
*
******************************************************************************/


#include "mvSetUpWindow.h"
#include "moveItBlob.h"

#include <windows.h>

using namespace std;
using namespace moveit;


int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(moveit);		// Resouces for moveIt (all the images are refered from here) 

	// Splash screen
	QApplication app(argc, argv);
	QPixmap *pixmap=new QPixmap(":/images/splash.png");
	QSplashScreen *splash=new QSplashScreen(*pixmap);
	splash->setMask(pixmap->mask());
	splash->show();

	// Splash screen dynamic text
	Qt::Alignment topRight = Qt::AlignCenter | Qt::AlignLeft ;
	splash->showMessage(QObject::tr("Setting up the main window...                                                                                                                                                                           "),topRight, Qt::white);
	Sleep(1000);	
	splash->showMessage(QObject::tr("Loading Codebook...                                                                                                                                                                                      "),topRight, Qt::white);
	Sleep(1000);

	// load codebook
	ifstream *mvFileCodebooke=new ifstream;
	mvFileCodebooke->open("codebook/democodebook.txt");


	// check whether codebook is open
	if (!mvFileCodebooke->is_open())
	{
		QMessageBox msgBox;
		msgBox.setText("Could not open codebook !");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
		return 0;
	}

	// load DHMM 
	DHMM * dhmm= new DHMM(7,79) ;
	int checkOpen=dhmm->Load("codebook/HMM.txt");

	// check whether DHMM is open
	if (checkOpen==1)
	{
		QMessageBox msgBox;
		msgBox.setText("Could not open HMM file !");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
		return 0;
	}

	splash->showMessage(QObject::tr("Establishing connections...                                                                                                                                                                             "),topRight, Qt::white);
	Sleep(2000);

	// Label
	QLabel *lineEdit= new QLabel("      Select the File");
	lineEdit->setFixedWidth(100);
	lineEdit->setFixedHeight(22);

	QGraphicsProxyWidget *txtBtnProxy = new QGraphicsProxyWidget;
	txtBtnProxy->setWidget(lineEdit);

	QGraphicsWidget *txtWidget = new QGraphicsWidget;
	QGraphicsGridLayout *txtLayout = new QGraphicsGridLayout(txtWidget);
	txtLayout->addItem(txtBtnProxy,0,1);
	txtWidget->setLayout(txtLayout);

	// Open button
	QPushButton *openBtn = new QPushButton;
	openBtn->setIcon(QIcon(":/images/open.png"));
	openBtn->setIconSize(QSize(90, 90));

	// Play button
	QPushButton *playBtn = new QPushButton;
	playBtn->setIcon(QIcon(":/images/demo.png"));
	playBtn->setIconSize(QSize(90, 90));
	playBtn->setEnabled(false);

	// Exit button
	QPushButton *exitBtn = new QPushButton;
	exitBtn->setIcon(QIcon(":/images/exit.png"));
	exitBtn->setIconSize(QSize(90, 90));

	// Graphics for buttons
	QGraphicsProxyWidget *openBtnProxy = new QGraphicsProxyWidget;
	openBtnProxy->setWidget(openBtn);

	QGraphicsProxyWidget *playBtnProxy = new QGraphicsProxyWidget;
	playBtnProxy->setWidget(playBtn);

	QGraphicsProxyWidget *exitBtnProxy = new QGraphicsProxyWidget;
	exitBtnProxy->setWidget(exitBtn);

	QGraphicsWidget *btnWidget = new QGraphicsWidget;
	QGraphicsGridLayout *btnLayout = new QGraphicsGridLayout(btnWidget);
	btnLayout->setSpacing(10);
	btnLayout->setColumnFixedWidth(0,50);
	btnLayout->setRowFixedHeight(0,10);

	btnLayout->setAlignment(openBtnProxy, Qt::AlignCenter);
	btnLayout->setAlignment(playBtnProxy, Qt::AlignCenter);
	btnLayout->setAlignment(exitBtnProxy, Qt::AlignCenter);

	// Alignment of Buttons
	btnLayout->addItem(openBtnProxy,1,1);
	btnLayout->addItem(playBtnProxy,1,2);
	btnLayout->addItem(exitBtnProxy,1,3);
	btnLayout->addItem(txtBtnProxy,2,2);
	btnWidget->setLayout(btnLayout);

	SetUpWindow mainWin(lineEdit,dhmm, mvFileCodebooke);
	mainWin.setButtons(playBtn, openBtn, exitBtn);

	QGraphicsScene scene(25,10,400,150);
	scene.setBackgroundBrush(scene.palette().window());
	scene.addItem(btnWidget);

	QGraphicsView view(&scene);
	view.setRenderHint(QPainter::Antialiasing);

	view.setBackgroundBrush(QPixmap(":/images/back.png"));
	view.setCacheMode(QGraphicsView::CacheBackground);
	view.setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
	view.setInteractive(true);
	view.setMaximumSize(450,170);
	view.setMinimumSize(450,170);
	view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "moveIt"));
	view.setGeometry(400,560,450,170);
	view.show();

	splash->finish(&view);
	delete splash;

	QObject::connect(openBtn, SIGNAL(clicked()), &mainWin, SLOT(openClicked()));// If Open button is clicked then execute openClicked() function
	QObject::connect(exitBtn, SIGNAL(clicked()), &app, SLOT(quit()));			// If Exit button is clicked then execute quit() function
	QObject::connect(playBtn, SIGNAL(clicked()), &mainWin, SLOT(playClicked()));// If Play button is clicked then execute playClicked() function

	return app.exec();
}
