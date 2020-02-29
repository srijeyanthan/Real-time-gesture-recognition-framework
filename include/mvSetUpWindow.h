/****************************************************************************
*
* mvSetUpWindow.h
*
* Purpose: This class handels information about GUI.
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
* [1]	Module name:	  setButtons
*		Purpose:		  Setup the buttons. 
*		Last update date: 23-06-2010
*
* [2]	Module name:	  enableButton
*		Purpose:		  Enable all three buttons. 
*		Last update date: 23-06-2010
*
* [3]	Module name:	  printMessage
*		Purpose:		  Provide messages for any Error. 
*		Last update date: 23-06-2010
*
* [4]	Module name:	  disableButton
*		Purpose:		  Disable all three buttons.  
*		Last update date: 23-06-2010
*
* [5]	Module name:	  setFileName
*		Purpose:		  Open the video file, get the file name. 
*		Last update date: 23-06-2010
*
* [6]	Module name:	  playClicked
*		Purpose:		  Detect whether play buttons is clicked or not. 
*		Last update date: 23-06-2010
*
* [7]	Module name:	  showPlay
*		Purpose:		  Start all the process.
*		Last update date: 23-06-2010
*
* [8]	Module name:	  openClicked
*		Purpose:		  Detect whether open buttons is clicked or not.
*		Last update date: 23-06-2010
*
******************************************************************************/


#pragma once

#include "mvDhmm.h"
#include<fstream>
#include <highgui.h>
#include <QtGui>

using namespace std;


enum Show
{
	Shadow, Bgs, Blob, Track, Skeleton, Demo
};


class SetUpWindow: public QGraphicsWidget
{
	Q_OBJECT

private:
	CvCapture *camera;		// Input file
	QString path;			// Actual path of file 
	QLabel *lineEdit;		// Contain file name
	QPushButton* playBtn;	// Play button
	QPushButton* openBtn;	// Open button
	QPushButton* exitBtn;	// Exit button
	DHMM *dhmm;				// DHMM
	ifstream *codebook;		// Code book
	QTimer *autoShootTimer;	// Timer


public:
	SetUpWindow(QLabel *lineEdit, DHMM * dhm, ifstream *mvFileCodebook, QGraphicsItem *parent=0);
	~SetUpWindow()
	{
		delete dhmm;
		codebook->close();
	}

	void setButtons(QPushButton* playBtn, QPushButton* openBtn, QPushButton* exitBtn);
	void enableButton();
	void printMessage(int i);
	void disableButton();

	public slots: 
		void setFileName();
		void playClicked();
		void showPlay();
		void openClicked();
};




