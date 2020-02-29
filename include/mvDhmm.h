/****************************************************************************************************************************
*
* dhmm.h
*
* Purpose: Implementation of Discrete Hidden Markov Model
* Author: Kokul
*
* Struct: State
* Modules: dhmm::State::SetObservationProbability
*					  ::SetTransitionProbability
*  					  ::NormalizeObservation
*					  ::NormalizeTransition
*					  ::Normalize
*					  ::Clear
*
*[1]	Module Name:	 SetObservztionProbability 
*		Purpose:		 To initialize the observation Probability to states
*		Last Update Date:21-06-2010
*
*[2]	Module Name:	 SetTransitionProbability
*		Purpose:		 To initialize the transition probabilities between states and observarion
*		Last Update Date:21-06-2010
*
*[3]	Module Name:	 NormalizeObservation
*		Purpose:		 To error correct the observation probabilities and rearrange the probability values if they are incorrect
*		Last Update Date:21-06-2010
*
*[4]	Module Name:	 NormalizeTransition
*		Purpose:		 To error correct the transition probabilities and rearrange the probability values if they are incorrect
*		Last Update Date:21-06-2010
*
*[5]	Module Name:	 Normalize
*		Purpose:		 To error correct the all probabilities and rearrange the probability values if they are incorrect
*		Last Update Date:21-06-2010
*
*[6]	Module Name:	 Clear
*		Purpose:		 To clear the state probability values
*		Last Update Date:21-06-2010
*
*
* Modules: dhmm::DHMM::GenerateObservation
*		 			 ::GenerateTransition
*					 ::GenerateFirstState
*					 ::Resize
*					 ::GetObservationSequence
*					 ::GetCurrentState
*					 ::AddState
*					 ::&GetState
*					 ::&operator
*					 ::Normalize
*					 ::Load
*					 ::GetStateSequenceProbability
*					 ::GetObservationSequenceProbability
*					 ::GetObservationSequenceProbabilityL
*					 ::GetProbableStateSequence
*					 ::GenerateSequence
*					 ::frand
*
*
*[1]	Module Name		:GenerateObservation
*		Purpose			:Create an observation probablity matrix
*		Last Update Date:21-06-2010
*
*[2]	Module Name		:GenerateTransition
*		Purpose			:Create a transition probabilty matrix
*		Last Update Date:21-06-2010
*
*[3]	Module Name		:GenerateFirstState
*		Purpose			:Generate the first state
*		Last Update Date:21-06-2010
*
*[4]	Module Name		:Resize
*		Purpose			:Resize the DHMM by adjusting the probability values
*		Last Update Date:21-06-2010
*
*[5]	Module Name		:GetObservationSequence
*		Purpose			:Get the observatio sequence to input into DHMM for find most suitable state
*		Last Update Date:21-06-2010
*
*[6]	Module Name		:GetCurrentState
*		Purpose			:Get the current state
*		Last Update Date:21-06-2010
*
*[7]	Module Name		:AddState
*		Purpose			:Add new state to DHMM
*		Last Update Date:21-06-2010
*
*[8]	Module Name		:&GetState
*		Purpose			:Get the state from given state number
*		Last Update Date:21-06-2010
*
*[9]	Module Name		:&operator
*		Last Update Date:21-06-2010
*
*[10]	Module Name		:Normalize
*		Purpose			:Normalize the all probability values
*		Last Update Date:21-06-2010
*
*[11]	Module Name		:Load
*		Purpose			:Load the DHMM by the inputs value from "HMM.txt"
*		Last Update Date:22-06-2010
*
*[12]	Module Name		:GetStateSequenceProbability
*		Purpose			:Get the state's transition probability matrix
*		Last Update Date:22-06-2010
*
*[13]	Module Name		:GetObservationSequenceProbability
*		Purpose			:Implementation of the Forward Backward Algorithm
*		Reference		:"Forward Backward Algorithm", 3 july, 2010. [Online]. 
*						 Available: http://en.wikipedia.org/wiki/Forward-backward_algorithm. 
*						 [Accessed: July 3, 2010].
*		Last Update Date:22-06-2010
*
*[14]	Module Name		:GetObservationSequenceProbabilityL
*		Purpose			:Implementation of the Forward Backward Algorithm
*		Reference		:"Forward Backward Algorithm"", 3 july, 2010. [Online]. 
*						 Available: http://en.wikipedia.org/wiki/Forward-backward_algorithm. 
*						 [Accessed: July 3, 2010].
*		Last Update Date:22-06-2010
*
*[15]	Module Name		:GetProbableStateSequence
*		Purpose			:Implementation of the Viterbi Algorithm
*		Reference		:"Viterbi Algorithm", 4 july, 2010. [Online]. 
*						 Available: http://en.wikipedia.org/wiki/Viterbi_algorithm. 
*						 [Accessed: July 4, 2010].
*		Last Update Date:22-06-2010
*
*[16]	Module Name		:GenerateSequence
*		Purpose			:Generate the sequence by analysing states
*		Last Update Date:22-06-2010
*
*[17]	Module Name		:frand
*		Purpose			:Generate a random value
*		Last Update Date:22-06-2010
*
*****************************************************************************************************************************/#pragma once

#include <vector>

using namespace std;
typedef unsigned int Symbol;
#define NoSymbol ((Symbol)(unsigned int)-1)
typedef vector<Symbol> Observation;
typedef vector<double> vectord;
typedef vector<int> vectori;



struct State
{
	State(unsigned numstates,unsigned numsymbols);
	unsigned NumStates;
	unsigned NumSymbols;
	unsigned ID;
	vector<double> ObservationProbability;
	vector<double> TransitionProbability;
	vector<bool> TransitionExist;
	virtual void Clear();
	virtual inline void SetObservationProbability(unsigned obs,double p) {ObservationProbability[obs]=p;};
	virtual void SetTransitionProbability(unsigned to,double p);
	virtual void NormalizeObservation();
	virtual void NormalizeTransition();
	virtual void Normalize(vector<double> &v);
};



class DHMM
{
public:

	unsigned NumStates;
	unsigned NumSymbols;
	unsigned CurrentState;
	vector <State> States;
	virtual Symbol GenerateObservation(State s);
	virtual unsigned GenerateTransition(State s);
	virtual unsigned GenerateFirstState();
	vector<vector<double> > alphamatrix;
	vector<vector<double> > betamatrix;
	vector<vector<double> > gammamatrix;
	DHMM(int numstates,int numsymbols);
	vector<double> StartingStateProbabilities;
	void GetObservationSequence(std::vector<int> obsSequence,DHMM *g);

	virtual ~DHMM();
	virtual void Resize(int numstates,int numsymbols);
	virtual unsigned GetCurrentState() {return CurrentState;};
	virtual void AddState(State s);
	virtual State &GetState(unsigned n);
	virtual State &operator[](unsigned n);
	virtual double Normalize(vector<double> &v, bool force=false);
	virtual void Normalize(vector<vector<double> > &v);
	virtual unsigned GetNumStates() { return NumStates;};
	virtual unsigned GetNumSymbols() { return NumSymbols;};
	virtual int Load(char *file);
	virtual double GetStateSequenceProbability(vector<unsigned int> s);
	virtual double GetObservationSequenceProbability(Observation obs);
	virtual double GetObservationSequenceProbabilityL(Observation obs);
	virtual Observation GetProbableStateSequence(Observation obs);
	virtual Observation GetProbableStateSequence(Observation obs,double &prob);
	virtual void Reset();
	virtual Observation GenerateSequence(int length,Observation &states);
	virtual Observation GenerateSequence(int length);
	virtual double frand();
};



