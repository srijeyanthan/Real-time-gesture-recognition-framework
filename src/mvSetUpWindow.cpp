/****************************************************************************
*
* mvSetUpWindow.cpp
*
* Purpose:  Implementation of mvSetUpWindow.h
*			This class handels information about GUI.
*
* Author: Nihanth
*
*
* Class: SetUpWindow
* Modules:	SetUpWindow::setButtons
* 					   ::enableButton
* 					   ::printMessage
* 					   ::disableButton
* 					   ::setFileName
* 					   ::playClicked
* 					   ::showPlay
* 					   ::openClicked
*
* Last update date: 12-06-2010
*
******************************************************************************/


#include "mvSetUpWindow.h"
#include "mvInitializeParallel.h"
#include <QMessageBox>


SetUpWindow::SetUpWindow(QLabel *line, DHMM * dhm,ifstream *mvFileCodebook, QGraphicsItem *parent) : 
QGraphicsWidget(parent) 
{
	camera=NULL;
	lineEdit=line;
	codebook=mvFileCodebook;
	dhmm=dhm;
}


//Detect whether open buttons is clicked or not.
void SetUpWindow::openClicked()
{
	autoShootTimer= new QTimer;
	autoShootTimer->start(100);
	connect(autoShootTimer, SIGNAL(timeout()), this, SLOT(setFileName()));

	if (!autoShootTimer->isActive())
	{
		return;
	}
}


//Detect whether play buttons is clicked or not. 
void SetUpWindow::playClicked()
{
	disableButton();
	autoShootTimer= new QTimer;
	autoShootTimer->start(500);
	connect(autoShootTimer, SIGNAL(timeout()), this, SLOT(showPlay()));

	if (!autoShootTimer->isActive())
	{
		return;
	}
}


//Start all the process.
void SetUpWindow::showPlay()
{
	InitializeParallel *init=new InitializeParallel;
	camera = cvCaptureFromAVI(path.toLatin1().data());
	int errorNo = init->runPipeline(camera,Shadow,dhmm,codebook);
	delete init;
	cvReleaseCapture(&camera);

	if (autoShootTimer->isActive())
	{
		autoShootTimer->stop();
	}

	printMessage(errorNo);
	enableButton();
}


//Provide messages for any Error. 
void SetUpWindow::printMessage(int i)
{
	QMessageBox msgBox;

	if(i==0){
		msgBox.setText("Could not open AVI file !");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
	} else 	if(i==1)
	{
		msgBox.setText("Incompatible video dimension !");
		msgBox.setInformativeText("Please input file with 320 x 240 dimension.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
	} else 	if(i==2)
	{
		msgBox.setText("Incompatible video frame rate !");
		msgBox.setInformativeText("Please input video with frame rate <30 \n for real time performance.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
	} else 	if(i==3)
	{
		msgBox.setText("Incompatible input file !");
		msgBox.setInformativeText("Please input .avi file.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
	} else if(i==5)
	{
		msgBox.setText("Codebook file is missing !");
		msgBox.setInformativeText("Please read the manual for further details.");
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.setIconPixmap(QPixmap(":/images/warning.png"));
		msgBox.setWindowTitle("moveIt : Warning");
		msgBox.exec();
	}
}


//Setup the buttons. 
void SetUpWindow::setButtons(QPushButton* play, QPushButton* open, QPushButton* exit)
{
	playBtn=play;
	openBtn=open;
	exitBtn=exit;
}


//Open the video file, get the file name. 
void SetUpWindow::setFileName()
{
	path= QFileDialog::getOpenFileName(0, "Open Video", "../moveIt/video", "AVI File (*.AVI)");

	if(path!=NULL)
	{
		std::string filePath=path.toStdString();
		size_t found;
		found=filePath.find_last_of("/\\");
		QString qstring = QString::fromStdString(filePath.substr(found+1));
		lineEdit->setText(  qstring );
		std::string fileName = qstring.toStdString();
		found=fileName.find_last_of("/.");
		QString extention = QString::fromStdString(fileName.substr(found+1));

		if(extention.toStdString().compare("avi") != 0)
		{
			printMessage(4);
		}
		enableButton();
	}

	if (autoShootTimer->isActive())
	{
		autoShootTimer->stop();
	}
}


//Enable all three buttons. 
void SetUpWindow::enableButton()
{
	playBtn->setEnabled(true);
	openBtn->setEnabled(true);
	exitBtn->setEnabled(true);
}


//Disable all three buttons.  
void SetUpWindow::disableButton()
{
	playBtn->setEnabled(false);
	openBtn->setEnabled(false);
	exitBtn->setEnabled(false);
}