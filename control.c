#include <control.h>
#include <math.h>


void GetStepVector(double * ctrlSignal, double acqTime, double sTime, double stepTime)
{
	long nSamples = lround(acqTime/sTime + 1);
	long prevSamples = lround(stepTime/sTime + 1);
	long i;
	// fill the step signal
	for(i = 0; i < nSamples; i++)
		ctrlSignal[i] = i < prevSamples?0:1;		
}

