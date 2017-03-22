#ifndef _CONTROL_H_
#define _CONTROL_H_


void GetStepVector(double * ctrlSignal, double acqTime, double sTime, double stepTime);
void GetTimeVector(double * timeVector, double acqTime, double sTime);

#endif
