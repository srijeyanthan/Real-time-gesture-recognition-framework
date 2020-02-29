
//#include <iostream>
#include <sstream>
#include <fstream>
//#include <cv.h>


#include "cvblob.h"
#define PI (3.1415926536)

namespace cvb{
	double a[] ={0.06,0.05,0.045,0.035,0.03};
	ifstream *myfileCodebook;
	DHMM * dhmm;
	ActionList list;
	CvFont font;
	//typedef std::map<int, int> Points;
	//typedef std::pair<int, int>  PointsPair;
	//typedef std::map<int, vector > trackPoints;
	//typedef std::pair<int, vector>  trackPointsPairs;

	ofstream txt("aa.txt",ios::app);

	void setFont(){
		txt<<"\n";
		cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, .5, .5, 0, 1, CV_AA);
	}
	void setCodebook(ifstream *codebook){
		myfileCodebook=codebook;
	}
	void setDhmm(DHMM * dhm){
		dhmm=dhm;
	}

	vector<CvPoint2D32f> drawskele(IplImage *grey, int x_, int y_, vector<CvPoint2D32f> vec,CvTrack *track,int frameNumber){

		vector<int> testx;
		vector<int> testy;
		vector<int> max;
		vector<int> distance;
		vector<double> real;
		vector<double> imaginary;
		vector<double> dft;
		vector<double> idft_real;
		vector<double> idft_imaginary;
		vector<double> idft;
		double localMaximumArray[10];
		int localMaxiumumArrayi[10];
		int count=0;

		IplImage* img_8uc1 = cvCreateImage( cvGetSize(grey), 8, 1 );
		IplImage* img_edge = cvCreateImage( cvGetSize(img_8uc1), 8, 1 );
		IplImage* img_8uc3 = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
		IplImage* test = cvCreateImage( cvGetSize(img_8uc1), 8, 3 );
		cvCvtColor( grey, img_8uc1, CV_BGR2GRAY );
		cvThreshold( img_8uc1, img_edge, 128, 255, CV_THRESH_BINARY );
		CvMemStorage* storage = cvCreateMemStorage();
		CvSeq* first_contour = NULL;

		int Nc = cvFindContours(
			img_edge,
			storage,
			&first_contour,
			sizeof(CvContour),
			CV_RETR_LIST);  

		CvScalar red = CV_RGB(250,0,0);
		CvScalar blue = CV_RGB(0,0,250);
		for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
		{
			if(c->h_next==NULL)
			{
				cvCvtColor( img_8uc1, img_8uc3, CV_GRAY2BGR );
				cvDrawContours(
					img_8uc3,
					c,
					red,		// Red
					blue,		// Blue
					1,			// Vary max_level and compare results
					2,
					8 );

				for( int i=0; i < c->total; ++i )
				{
					CvPoint* p = CV_GET_SEQ_ELEM( CvPoint, c, i );
					testx.push_back(p->x);
					testy.push_back(p->y);
				}
			}
		}

		double dis =0.0;
		for(int t=0;t<testx.size();++t)
		{
			dis =sqrt((double)((testx[t]-x_)*(testx[t]-x_))+(double)((testy[t]-y_)*(testy[t]-y_)));
			distance.push_back(dis);
		}

		int total_size =distance.size();
		int k1;
		int m1;
		for( k1=0;k1<total_size/2;++k1)
		{
			double real_final=0.0;
			double imag_final=0.0;
			for( m1=0;m1<total_size-1;++m1)
			{
				real_final=distance[m1]*cos(PI*k1*m1/total_size)+real_final;
				imag_final=distance[m1]*sin(PI*k1*m1/total_size)+imag_final;
			}
			real.push_back(real_final);
			imaginary.push_back(imag_final);
		}

		double final_dft=0.0;
		for(int r=0;r<real.size();++r)
		{
			final_dft = sqrt( real[r]*real[r] + imaginary[r]*imaginary[r]);
			dft.push_back(final_dft);
		}

		double psmax=0.0;
		for(int i = 0; i < dft.size(); ++i) 
		{
			if(dft[i] > psmax)
				psmax = dft[i];
		}

		for(int i = 0; i < dft.size(); ++i) 
		{
			dft[i] = dft[i]/psmax;
		}

		for(int g=0;g<4;++g)
		{
			for( k1=0;k1<total_size-1;++k1)
			{
				double real_idft_final=0.0;
				double imag_dft_final=0.0;
				for( m1=0;m1<(a[g]*distance.size() );++m1)//0.05*distance.size()
				{
					real_idft_final=(real[m1]*cos(PI*k1*m1/total_size)+imaginary[m1]*sin(PI*k1*m1/total_size))+real_idft_final;
					imag_dft_final=(real[m1]*sin(PI*k1*m1/total_size)-imaginary[m1]*cos(PI*k1*m1/total_size))+imag_dft_final;
				}
				idft_real.push_back(real_idft_final/total_size);
				idft_imaginary.push_back(imag_dft_final/total_size);
			}

			double final_idft=0.0;
			for(int r=0;r<idft_real.size();++r)
			{
				final_idft = sqrt( idft_real[r]*idft_real[r] + idft_imaginary[r]*idft_imaginary[r]);
				idft.push_back(final_idft);
			}

			int maximum_x_value=idft.size(); 
			for(int j=3; j<maximum_x_value-3;j++)
			{
				for(int k=j-3; k<j+4; k++)
				{
					if(k>j)
					{
						if(idft[j]<idft[k]) break;
					} else if(k<j)
					{
						if(idft[j]<idft[k])break;
					}
					if(k==j+3)
					{
						localMaximumArray[count]= idft[j];
						localMaxiumumArrayi[count]=j;
						count+=1;				
					}
					if(count>6)
						break;
				}
			}

			if(count>6)
			{
				count =0;
				for(int i=0;i<10;++i)
				{
					localMaximumArray[i]= 0;
					localMaxiumumArrayi[i]=0;
				}
				continue;
			}
			else
				break;
		}
		// check if else insted of if
		if((count==3) && (abs(total_size-localMaxiumumArrayi[2]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[2]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[3]))	
		{	
			CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);
			vec.push_back(p);
			CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
			vec.push_back(p2);
			sequenceGenerator(vec,  x_,  y_,track,frameNumber);  
		}

		if(count==4)
		{
			if(abs(total_size-localMaxiumumArrayi[3]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[3]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[3])	
			{	
				CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);//testx[0], testy[0]
				vec.push_back(p);
				CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
				vec.push_back(p2);
				CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
				vec.push_back(p3);	
				sequenceGenerator(vec, x_,  y_,track,frameNumber);        
			}
		}

		if(count==5)
		{
			if(abs(total_size-localMaxiumumArrayi[4]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[4]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[4])	
			{	
				CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);//testx[0], testy[0]
				vec.push_back(p);
				CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
				vec.push_back(p2);
				CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
				vec.push_back(p3);
				CvPoint2D32f p4=cvPoint2D32f(testx[localMaxiumumArrayi[3]], testy[localMaxiumumArrayi[3]]);
				vec.push_back(p4);
				sequenceGenerator(vec,  x_, y_,track,frameNumber);        
			}
		}

		if(count==6)
		{
			if(abs(total_size-localMaxiumumArrayi[5]-localMaxiumumArrayi[0])<localMaxiumumArrayi[0] || abs(total_size-localMaxiumumArrayi[5]-localMaxiumumArrayi[0])<total_size-localMaxiumumArrayi[5])	
			{	
				CvPoint2D32f p=cvPoint2D32f(testx[0],testy[0]);//testx[0], testy[0]
				vec.push_back(p);
				CvPoint2D32f p2=cvPoint2D32f(testx[localMaxiumumArrayi[1]], testy[localMaxiumumArrayi[1]]);
				vec.push_back(p2);
				CvPoint2D32f p3=cvPoint2D32f(testx[localMaxiumumArrayi[2]], testy[localMaxiumumArrayi[2]]);
				vec.push_back(p3);
				CvPoint2D32f p4=cvPoint2D32f(testx[localMaxiumumArrayi[3]], testy[localMaxiumumArrayi[3]]);
				vec.push_back(p4);
				CvPoint2D32f p5=cvPoint2D32f(testx[localMaxiumumArrayi[4]], testy[localMaxiumumArrayi[4]]);
				vec.push_back(p5);
				sequenceGenerator(vec,  x_, y_,track,frameNumber);        
			}
		}

		testx.clear();
		testy.clear();
		max.clear();
		distance.clear();
		real.clear();
		imaginary.clear();
		dft.clear();
		idft_real.clear();
		idft_imaginary.clear();
		idft.clear();
		cvReleaseImage(&img_8uc1);
		cvReleaseImage(&img_edge );
		cvReleaseImage(&img_8uc3 );
		cvReleaseImage(&test );
		cvClearMemStorage(storage );
		cvClearSeq(first_contour );
		return vec;

	}



	void sequenceGenerator(vector <CvPoint2D32f > points,int bar_x, int bar_y,CvTrack *track,int frameNumber)
	{
		vector <string> test;
		string line ;
		vector <double> min;
		vector <double> tempmin;
		vector <int> posmin;
		vector <CvPoint2D32f > temp;
		int x_bar;
		int y_bar;
		double d=0.0;
		int maincount=0;
		vector <int> subcount;//where is clear
		int seq=0;

		if (myfileCodebook->is_open())
		{
			while (! myfileCodebook->eof() )
			{
				getline ((*myfileCodebook),line);
				test.push_back(line);
				if(line=="0")
				{ 
					maincount =maincount+1;

					if(test.size()== (2*points.size()+3))
					{
						subcount.push_back(maincount);
						for(int i=0;i<=points.size();++i)
						{
							if(i==points.size())
							{
								x_bar= atoi(test[2*i].c_str());
								y_bar= atoi(test[2*i+1].c_str());
							}
							else
							{
								CvPoint2D32f p=cvPoint2D32f(atoi(test[2*i].c_str()), atoi(test[2*i+1].c_str()));
								temp.push_back(p);
							}
						}
						double mintotal=0.0;
						tempmin.clear();

						for(int i=0;i<points.size();++i)
						{
							for(int j=0;j<points.size();++j)
							{
								d= sqrt((temp[i].x-(points[j].x+x_bar-bar_x))*(temp[i].x-(points[j].x+x_bar-bar_x)) +(temp[i].y-(points[j].y+y_bar-bar_y))*(temp[i].y-(points[j].y+y_bar-bar_y)));
								tempmin.push_back(d);
							}
							posmin.push_back(findMinPos(tempmin));
							//cehck if else insted of if
							if(i==0)
							{
								mintotal= mintotal+findMin(tempmin);
							}
							if(i==1 && posmin[1]==posmin[0])
							{

								tempmin[posmin[1]]=1000;
								mintotal= mintotal+findMin(tempmin);
							}
							if(i==2 && (posmin[2]==posmin[0] ||posmin[2]==posmin[1]) )
							{

								tempmin[posmin[1]]=1000;
								tempmin[posmin[0]]=1000;

								mintotal= mintotal+findMin(tempmin);
							}
							if(i==3 && (posmin[3]==posmin[2] ||posmin[3]==posmin[1]||posmin[3]==posmin[0]) )
							{

								tempmin[posmin[2]]=1000;
								tempmin[posmin[1]]=1000;
								tempmin[posmin[0]]=1000;

								mintotal= mintotal+findMin(tempmin);
							}
							if(i==4 && (posmin[4]==posmin[3] ||posmin[4]==posmin[2]||posmin[4]==posmin[1]||posmin[4]==posmin[0]) )
							{
								tempmin[posmin[3]]=1000;
								tempmin[posmin[2]]=1000;
								tempmin[posmin[1]]=1000;
								tempmin[posmin[0]]=1000;
								mintotal= mintotal+findMin(tempmin);
							}
						}
						min.push_back(mintotal);	
						temp.clear();
						tempmin.clear();
						posmin.clear();
					}
					test.clear();
				}
			}
			if(subcount.size() != 0)
			{
				seq = subcount[findMinPos(min)];
				//cout<<" possible match .... "<<seq-1<<endl;
				txt<<seq-1<<"\n";
				track->actionPoints->insert(pair<int,int>(frameNumber%10,(seq-1)));

			}

			if(track->actionPoints->size()==10)
			{
				track->action="";
				int obsSize = 10;
				Observation obs;
				obs.resize(obsSize,0);
				//cout<<"Observation Sequence"<< endl;
				for (int i = 0; i<obsSize; i++)
				{
					obs[i]= (*track->actionPoints)[i];
					//cout<< obs[i]<<endl;
				}

				Observation obsx = dhmm->GetProbableStateSequence(obs);
				int noOfGest[4] = {0,0,0,0};
				for (int  j=0; j< obsSize; j++)
				{
					switch (obsx[j])
					{
					case 0 : noOfGest[0]++;break;
					case 1 : noOfGest[1]++;break;
					case 2 : noOfGest[2]++;break;
					case 3 : noOfGest[3]++;break;
					}
				}

				int mostProbable = 0;
				int maxGest = 0;
				for (int k =0; k<4; k++)
				{
					if (noOfGest[k] > maxGest)
					{
						maxGest =noOfGest[k];
						mostProbable = k;
					}
				}

				//change if to if else
				if(mostProbable==0)
				{
					track->action="walking";

				}else if(mostProbable==1)
				{
					track->action="running";
				}else if(mostProbable==2)
				{
					track->action="helping";
				} else 	if(mostProbable==3){
					track->action="sitting";
				} 
				//list.clear();
				//if(track->id >1)
				//	std::cout<<""<<std::endl;
				//list.insert(ActionListPair(track->id,track->action));

				list[track->id]=track->action;
				//track->action=list.find(track->id)->second;
				//list.insert(track->id,track->action);
				track->actionPoints->clear();
			}else{
				//if(te==true)
				//errorr check nikkey
				//int x=list.find(track->id)->first;
				if( list.count(track->id)>0)
					track->action=list.find(track->id)->second;
				//list.insert(ActionListPair(track->id,track->action));
			}
			min.clear();
			myfileCodebook->clear();
			myfileCodebook->seekg(0,ios_base::beg);
		} else 
			throw 5;
	}




	double findMin( std::vector<double> minx)
	{
		double minimum = minx[0];
		for(int j=0;j<minx.size();++j)
		{
			if(minx[j]<minimum)
				minimum = minx[j];
		}

		return minimum;
	}

	int  findMinPos(std::vector<double> minp)
	{
		double minimum = minp[0];
		int pos =0;
		for(int j=0;j<minp.size();++j)
		{
			if(minp[j]<minimum)
			{
				minimum = minp[j];
				pos =j;
			}	 
		}
		return pos;
	}




	int findSkle(CvTracks *tracks,IplImage *bgs,IplImage* frame,int frameNumber){
		vector <CvPoint2D32f > currentpoints;
		for (CvTracks::const_iterator jt = tracks->begin(); jt!=tracks->end();jt++){

			if(jt->second->merge!=true){
				cvSetImageROItoTracks(bgs,jt->second);

				try{
					currentpoints = drawskele(bgs, (jt->second->centroid.x - jt->second->minx), (jt->second->centroid.y - jt->second->miny),currentpoints,jt->second,frameNumber);
				}catch(...){
					return 5;
				}
				cvResetImageROI(bgs);
				stringstream buffer;
				buffer << jt->second->action; 
				cvPutText(frame, buffer.str().c_str(), cvPoint((int)jt->second->minx,(int)jt->second->miny-5), &font, CV_RGB(0.,255.,0.));

				for(int i=0; i<currentpoints.size();i++){
					cvLine(frame, cvPoint(jt->second->centroid.x,jt->second->centroid.y),   cvPoint(currentpoints[i].x + jt->second->minx, currentpoints[i].y + jt->second->miny),  cvScalar(255, 255, 0, 0), 3, 8); 
				}
				currentpoints.clear();
			}
		}

		return -1;
	}
}