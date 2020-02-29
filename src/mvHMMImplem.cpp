/****************************************************************************************************************************
*
*HMMImplem.cpp
*
*Purpose: Implementation of Discrete Hidden Markov Model
*Author: Kokul
*Struct: State
*Modules: dhmm::State::SetObservationProbability
*					 :: SetTransitionProbability
*					 :: NormalizeObservation
*					 :: NormalizeTransition
*					 :: Normalize
*					 :: Clear
*
*Modules: dhmm::DHMM:: GenerateObservation
*					:: GenerateTransition
*					:: GenerateFirstState
*					:: Resize
*					:: GetObservationSequence
*					:: GetCurrentState
*					:: AddState
*					:: &GetState
*					:: &operator
*					:: Normalize
*					:: Load
*					:: GetStateSequenceProbability
*					:: GetObservationSequenceProbability
*					:: GetObservationSequenceProbabilityL
*					:: GetProbableStateSequence
*					:: GenerateSequence
*					:: frand
*
*Last update date: 23-06-2010
*
*******************************************************************************************************************************/


#include <string>
#include <cxcore.h>
#include <cv.h>
#include <cvaux.h>
#include <vector>
#include <math.h>
#include <fstream>

#include "map"
#include "iostream"
#include "mvDhmm.h"

using namespace std;


State::State(unsigned numstates,unsigned numsymbols)
{
	NumStates = numstates;
	NumSymbols = numsymbols;
	Clear();
}


//To initialize the transition probabilities between states and observarion
void State::SetTransitionProbability(unsigned to,double p) 
{
	// Distinguish existing low probability transitions (Exist=true) and no transitions (Exist=false).
	TransitionProbability[to] = p;
	TransitionExist[to] = true;
};


// To clear the state probability values
void State::Clear()
{
	ObservationProbability.clear();
	ObservationProbability.resize(NumSymbols,0.0);

	TransitionProbability.clear();
	TransitionProbability.resize(NumStates,0);
	TransitionExist.clear();
	TransitionExist.resize(NumStates,false);
}


//To error correct the observation probabilities and rearrange the probability values if they are incorrect
void State::NormalizeObservation()
{
	Normalize(ObservationProbability);
}


//To error correct the transition probabilities and rearrange the probability values if they are incorrect
void State::NormalizeTransition()
{
	Normalize(TransitionProbability);
}


//To error correct the all probabilities and rearrange the probability values if they are incorrect
void State::Normalize(vector<double> &v)
{
	double sum=0;
	double mult;
	for(unsigned i=0;i<v.size();i++)
		sum+=v[i];
	if(sum!=0)
	{
		mult = 1.0/sum;
		for(unsigned i=0;i<v.size();i++)
			v[i]=v[i]*mult;
	}
}


DHMM::DHMM(int numstates,int numsymbols)
{
	Resize(numstates,numsymbols);
}


DHMM::~DHMM()
{
}


//Resize the DHMM by adjusting the probability values
void DHMM::Resize(int numstates,int numsymbols)
{
	NumStates = numstates;
	NumSymbols = numsymbols;
	CurrentState=0;   
	States.clear();
	States.resize(numstates,State(numstates,numsymbols));
	for(unsigned i=0;i<NumStates;i++)
		States[i].ID=i;
	StartingStateProbabilities.clear();
	StartingStateProbabilities.resize(numstates,0);
	StartingStateProbabilities[0]=1.0;
}


// Reset the current state to 0
void DHMM::Reset()
{
	CurrentState=0;
}


//Create an observation probablity matrix
Symbol DHMM::GenerateObservation(State s)
{
	double r;
	double cr;
	unsigned i;

	if(s.ObservationProbability.size()==0)
	{
		return NoSymbol;
	}
	r = frand();
	for(i=0, cr=0.0;i<s.ObservationProbability.size();i++)
	{
		cr += s.ObservationProbability[i];
		if(r<cr)
		{
			return (Symbol)i;
		}
	}
	return (Symbol)i-1; 
}


//Create a transition probabilty matrix
unsigned DHMM::GenerateTransition(State s)
{
	double r;
	double cr;
	unsigned i;

	if(s.TransitionProbability.size()==0)
	{
		return NoSymbol;
	}
	r = frand();
	for(i=0, cr=0.0;i<s.TransitionProbability.size();i++)
	{
		if(s.TransitionExist[i] == false)
			continue;
		cr += s.TransitionProbability[i];
		if(r<cr)
		{
			return i;                         
		}
	}
	return i-1;

}


//Generate the first state
unsigned DHMM::GenerateFirstState()
{
	double r;
	double cr;
	unsigned i;

	r = frand();
	for(i=0, cr=0.0;i<StartingStateProbabilities.size();i++)
	{
		cr += StartingStateProbabilities[i];
		if(r<cr)
			return i;
	}
	return i-1;
}


//Generate the sequence by analysing states
Observation DHMM::GenerateSequence(int length,Observation &states)
{
	Observation obs;
	Symbol s;

	states.clear();

	obs.resize(length,0);
	states.resize(length,0);

	// Set the starting state.
	CurrentState = GenerateFirstState();

	// Generate the sequence starting from CurrentState
	for(int i=0;i<length;i++)
	{
		// Store the state which generates the observation
		states[i]=CurrentState;

		// Generate an observation
		obs[i]=GenerateObservation(States[CurrentState]);

		// Generate a transition
		CurrentState = GenerateTransition(States[CurrentState]);

	}
	return obs;
}


//Generate the sequence by analysing states
Observation DHMM::GenerateSequence(int length)
{
	Observation states;
	return GenerateSequence(length,states);
}


//Generate a random value
double DHMM::frand()
{
	double r;
	r=((double)rand())/((double)RAND_MAX);
	return r;
}


//Add new state to DHMM
void DHMM::AddState(State s)
{
	States[s.ID] = s;
}


//Get the state from given state number
State &DHMM::GetState(unsigned n)
{
	return States[n];
}


State &DHMM::operator [](unsigned n)
{
	return States[n];
}


//Get the state's transition probability matrix
double DHMM::GetStateSequenceProbability(vector<unsigned int> seq)
{
	double p;

	p = StartingStateProbabilities[seq[0]];
	for(unsigned i=1;i<seq.size();i++)
	{
		//find in state s the transition to state id s[i].
		State &s = operator[](seq[i-1]);       // Get the previous state
		if(s.TransitionExist[seq[i]] == false)
		{
			return 0.0;
		}

		p*=s.TransitionProbability[seq[i]]; 
	}
	return p;
}


//Implementation of the Forward Backward Algorithm
double DHMM::GetObservationSequenceProbability(Observation obs)
{
	vector<double> alpha,alpha2;      // alpha: forward variable.
	double tp;

	// alpha[i]: probability of observing the correct symbols up to current time
	alpha.resize(NumStates,0.0);
	alpha2.resize(NumStates,0.0);

	// Initialization
	for(unsigned i=0;i<NumStates;i++)
		alpha[i] = StartingStateProbabilities[i]*operator[](i).ObservationProbability[obs[0]];

	//Induction: find probabilities for time T+1 from time T
	for(unsigned t=1;t<obs.size();t++)        // Iterate all the observations (i.e. time steps)
	{
		for(unsigned i=0;i<NumStates;i++)      // Iterate the states at T+1
		{
			tp=0;
			for(unsigned j=0;j<NumStates;j++)   // Iterate the states at T
				tp+=alpha[j] * operator[](j).TransitionProbability[i];

			alpha2[i] = tp*operator[](i).ObservationProbability[obs[t]];
		}
		for(unsigned i=0;i<NumStates;i++)
			alpha[i]=alpha2[i];
	}
	//Termination: collect all the probabilities in alpha
	tp=0;
	for(unsigned i=0;i<NumStates;i++)
		tp+=alpha[i];

	return tp;
};


//Implementation of the Forward Backward Algorithm
double DHMM::GetObservationSequenceProbabilityL(Observation obs)
{
	vector<double> alpha,alpha2;
	double tp;
	double scale;
	double logscale;

	// alpha[i]: probability of observing the correct symbols up to current time
	alpha.resize(NumStates,0.0);
	alpha2.resize(NumStates,0.0);
	scale = 1.0;
	logscale=0.0;

	// Initialization
	for(unsigned i=0;i<NumStates;i++)
		alpha[i] = StartingStateProbabilities[i]*operator[](i).ObservationProbability[obs[0]];
	//Induction: find probabilities for time T+1 from time T
	for(unsigned t=1;t<obs.size();t++)        // Iterate all the observations (i.e. time steps)
	{
		for(unsigned i=0;i<NumStates;i++)      // Iterate the states at T+1
		{
			tp=0;
			for(unsigned j=0;j<NumStates;j++)   // Iterate the states at T
				tp+=alpha[j] * operator[](j).TransitionProbability[i];

			alpha2[i] = tp*operator[](i).ObservationProbability[obs[t]];
		}
		for(unsigned i=0;i<NumStates;i++)
			alpha[i]=alpha2[i];
		double tt=Normalize(alpha);
		logscale -= log(tt);
	}
	double tt=Normalize(alpha,true);
	scale*=tt;
	logscale-=log(tt);
	return logscale;
};


//Implementation of the Viterbi Algorithm
Observation DHMM::GetProbableStateSequence(Observation obs,double &prob)
{
	Observation Path;
	vector<double> delta,delta2;
	vector<vector<unsigned> > psi;
	// delta t(i): best score (highest probability) along a single path at time t ending in state i
	delta.resize(NumStates,0.0);
	delta2.resize(NumStates,0.0);

	psi.resize(obs.size(),vector<unsigned>(NumStates,0));
	// Initialisation
	for(unsigned i=0;i<NumStates;i++)
	{
		delta[i] = StartingStateProbabilities[i]*operator[](i).ObservationProbability[obs[0]];
		psi[0][i] = 0;
	}
	for(unsigned t=1;t<obs.size();t++)        // Iterate the observations (time steps)
	{
		for(unsigned i=0;i<NumStates;i++)      // Iterates the states at T+1
		{
			double p;
			unsigned argmax;

			p = -1;
			for(unsigned j=0;j<NumStates;j++)   // Find the most likely transition
			{
				double pt;
				pt = delta[j]*operator[](j).TransitionProbability[i];
				if(pt>p)
				{
					p=pt;
					argmax=j;
				}
			}
			// Store the highest probability
			delta2[i] = p*operator[](i).ObservationProbability[obs[t]];

			// Store the trace
			psi[t][i] = argmax;
		}
		for(unsigned i=0;i<NumStates;i++)
			delta[i]=delta2[i];
	}

	// Termination.
	//probabilities of all the path ending at time T in state i with correct observations; select the last hop.
	double P;                  // Overall probability of the state sequence
	unsigned argmax;           // Ending state
	P=-1;
	for(unsigned i=0;i<NumStates;i++)
	{
		if(delta[i]>P)
		{
			P=delta[i];
			argmax=i;
		}
	}

	// Path backtracking
	Path.resize(obs.size(),0);
	// psi[0] does not exist! data are [psi(1), psi(2)....psi(obs.size()), argmax].
	for(unsigned t=obs.size()-1;t>=1;t--)
	{
		Path[t] = argmax;
		argmax = psi[t][argmax];
	}
	Path[0]=argmax;
	prob=P;
	return Path;
}


//Implementation of the Viterbi Algorithm
Observation DHMM::GetProbableStateSequence(Observation obs)
{
	double p;
	return GetProbableStateSequence(obs,p);
}


//To error correct the all probabilities and rearrange the probability values if they are incorrect
double DHMM::Normalize(vector<double> &v,bool force)
{
	double sum=0;
	double mult;
	if(0)
	{
		for(unsigned i=0;i<v.size();i++)
			sum+=v[i];
		if(sum!=0)
		{
			mult = 1.0/sum;				
			for(unsigned i=0;i<v.size();i++)
				v[i]=v[i]*mult;
			return mult;
		}
		else
			return 1.0;
	}
	else
	{
		for(unsigned i=0;i<v.size();i++)
			sum+=v[i];

		if( sum!=0 )
		{
			if(force==true)
			{
				mult = 1.0/sum;
				for(unsigned i=0;i<v.size();i++)
					v[i]=v[i]*mult;
				return mult;
			}
			mult=1.0;
			while(sum<0.001)                          
			{
				for(unsigned i=0;i<v.size();i++)
					v[i]=v[i]*100;
				mult*=100;
				sum*=100;
			}
			return mult;
		}
		return 1.0;
	}
}


//To error correct the all probabilities and rearrange the probability values if they are incorrect
void DHMM::Normalize(vector<vector<double> > &v)
{
	for(unsigned i=0;i<v.size();i++)
		Normalize(v[i],true);	
}


// Load the DHMM by the inputs value from "HMM.txt"
int DHMM::Load(char *file)
{
	double t;
	int ti;
	FILE *f = fopen(file,"rt"); 
	if(f==NULL)
		return 1;
	fscanf(f,"%d %d\n",&NumStates,&NumSymbols);
	Resize(NumStates,NumSymbols);

	for(unsigned i=0;i<StartingStateProbabilities.size();i++)
	{
		fscanf(f,"%lf ",&t);
		StartingStateProbabilities[i]=t;
	}
	for(unsigned s=0;s<NumStates;s++)
	{
		fscanf(f,"%d\n",&operator[](s).ID);
		for(unsigned i=0;i<operator[](s).ObservationProbability.size();i++)
		{
			fscanf(f,"%lf ",&t);
			operator[](s).ObservationProbability[i]=t;
		}
		for(unsigned i=0;i<operator[](s).TransitionProbability.size();i++)
		{
			fscanf(f,"%lf ",&t);
			operator[](s).TransitionProbability[i]=t;
		}
		for(unsigned i=0;i<operator[](s).TransitionExist.size();i++)
		{
			fscanf(f,"%d ",&ti);
			operator[](s).TransitionExist[i]=ti;
		}
	}	
	fclose(f);
	return -1;
}