#include <control.h>
#include <math.h>


void GetStepVector(double * ctrlSignal, double acqTime, double sTime, double stepTime)
{
	long nSamples = lround(acqTime/sTime) + 1;
	long prevSamples = lround(stepTime/sTime) + 1;
	long i;
	
	// fill the step signal
	for(i = 0; i < nSamples; i++)
		ctrlSignal[i] = i < prevSamples?0:1;		
}

void GetTimeVector(double * timeVector, double acqTime, double sTime)
{
	long nSamples = lround(acqTime/sTime) + 1;
	long i;
	double acu = 0.0;
	
	// fill the time array
	for(i = 0; i < nSamples; i++)
	{	
		acu += sTime;
		timeVector[i] = acu; 	
	}
}



