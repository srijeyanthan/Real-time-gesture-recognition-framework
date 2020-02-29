/****************************************************************************
*
* mvTrackObj.h
*
* Purpose: Class that contain information about Track Object. 
*		   - Properties of Track Object: 
*		   		action			
*		   		height			
*		   		beginX			
*		   		lastX			
*		   		actionPoints	
*		   		behaviour		
*		   		uncommon		
*
* Author: Nihanth
*
*
* Class: TrackObj
*
* Last update date: 22-06-2010
*
******************************************************************************/


#pragma once

class TrackObj
{

public:
	struct
	{
		string action;							// Tracked object's action.
		unsigned int height;					// Tracked object's height.
		unsigned int beginX;					// X value of centroid - at initial stage.
		unsigned int lastX;						// X value of centroid - at last.
		vector <unsigned int> actionPoints;		// Action points of Postures.
		vector <int> behaviour;					// Gesture of track object.
		vector <int> uncommon;					// Detect un common behaviour.
		int uncommonpopvalue;					
		bool abnorm;							
	};

};