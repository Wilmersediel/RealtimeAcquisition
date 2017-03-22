#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <control.h>
#include <SaveData.h>
#include <dUQx.h>

#define N_INPUT_ARGS    6
#define FILE_NAME_POS   1
#define ACQ_TIME_POS    2
#define SAMPLE_TIME_POS 3
#define STEP_TIME_POS	4
#define USB_PORT_POS    5


uint8_t ch = 0;
double vref = 0.0;

int AcquisitionEngine(char *filename, double acqTime, double sTime, double stepTime, int usbPort);



int main(int argn, char **argv)
{
	double acqTime   = 0.0;
	double sTime     = 0.0;
	double stepTime  = 0.0;
	int usbPort      = 16; //defaults to ttyUSB0
	char * end;
	
	
	if(!(argn == N_INPUT_ARGS))
	{
		if(argn == 2 && !strcmp(argv[1], "--help"))
			printf("This command gets: fileName acqTime sampleTime stepTime usbPortNumber\n");
		else	
			printf("This application requires %d input arguments...\nLeaving\n", N_INPUT_ARGS - 1);
		return 1;
	}
	
	acqTime = strtod(argv[ACQ_TIME_POS], &end);
	if(end == argv[ACQ_TIME_POS])
	{
		printf("Inconsistent acquisition time.\nLeaving.\n");
		return 2;
	}
	
	sTime = strtod(argv[SAMPLE_TIME_POS], &end);
	if(end == argv[SAMPLE_TIME_POS])
	{
		printf("Inconsistent sample time.\nLeaving.\n");
		return 2;
	}
	
	stepTime = strtod(argv[STEP_TIME_POS], &end);
	if(end == argv[STEP_TIME_POS])
	{
		printf("Inconsistent step-time.\nLeaving.\n");
		return 2;
	}
	
	usbPort = (int)strtod(argv[USB_PORT_POS], &end);
	if(end == argv[USB_PORT_POS])
	{
		printf("Inconsistent UsbPort number\nLeaving\n");
		return 2;
	}
	return AcquisitionEngine(argv[FILE_NAME_POS], acqTime, sTime, stepTime, usbPort);
}


int InitDuqx(int usbPort)
{
	//Inits dUQx board
	if(dUQx_Init(usbPort) != 0) //24 -> For /dev/ttyACM0 in linux
	{			           		//16 -> For /dev/ttyUSB0 in linux
		printf("Can not connect with dUQx board\n");
		return 3;
	}	
	//Sets 10-bit resolution
	dUQX_SetResolution(1);
	
	//Digital port is configured as output
	dUQx_ConfigureDigital(0x3ff);

	//Writes 000h number on digital port
	dUQx_WriteDigital(0x000);
	//Estimates reference voltage
	dUQx_CalibrateAnalog(&vref);
	return 0;
}

double GetSample()
{
	double v;
	//Reads input analog channel 0
	dUQx_ReadAnalogSingle(ch,vref,&v);
	return v;
}



int AcquisitionEngine(char *filename, double acqTime, double sTime, double stepTime, int usbPort)
{
	// acqTime, sTime & stepTime in milliseconds
	long nSamples = lround(acqTime/sTime) + 1;
	long sTimeSecs = (long)sTime / 1000;
	long sTimeNanosecs = (long)round(((sTime/1000)-sTimeSecs) * 1000000000L);
	double * timeArray = (double *)malloc(sizeof(double)*nSamples);
	double * inputData = (double *)malloc(sizeof(double)*nSamples); // store the data that comes throug the serial port
	double * controlSig= (double *)malloc(sizeof(double)*nSamples);
	
	GetStepVector(controlSig, acqTime, sTime, stepTime);
	GetTimeVector(timeArray, acqTime, sTime);
	int received_sig, sampleCounter = 0;
	timer_t sampling_timer;
	struct itimerspec timer_period;
	struct sigevent timer_event;
	
	// Receiver for SIGALRM signal
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_BLOCK, &set, NULL);	
	
	// Starts dUQx
	if(InitDuqx(usbPort) == 3)
		return 3;
		
	// Setup the handler
	timer_event.sigev_notify = SIGEV_SIGNAL;	
	timer_event.sigev_signo = SIGALRM;
	
	
	// Create the timer
	timer_create(CLOCK_REALTIME, &timer_event, &sampling_timer);
	
	// Se configura el periodo de disparo del timer
	timer_period.it_value.tv_sec     = sTimeSecs;
	timer_period.it_value.tv_nsec    = sTimeNanosecs;
	// Los atributos iterval hacen referencia al periodo de deisparo del timer (si son 0 solo ocurre la primera vez)
	timer_period.it_interval.tv_sec  = sTimeSecs;
	timer_period.it_interval.tv_nsec = sTimeNanosecs;
	
	// Starts the timer
	timer_settime(sampling_timer, 0, &timer_period, NULL);	
	
	while(sampleCounter < nSamples)
	{
		// Gets one sample
		inputData[sampleCounter] = GetSample();
		printf("inputData[%d] = %lf\n",sampleCounter,inputData[sampleCounter]);
		// Send the step signal
		dUQx_WriteDigital((uint16_t)controlSig[sampleCounter]);
		sigwait(&set, &received_sig);
		sampleCounter++;
	}
	printf("sampleCounter = %d\n",sampleCounter);
	
	// Saves the info with sampleCounter to avoid storing garbage
	if(SaveData(filename, timeArray, controlSig, inputData, sampleCounter) == 1)
		return 4;  //a problem has occurred while saving the file
	
	//Closes dUQx
	dUQx_End();
	
	// free the memory of data arrays
	free(timeArray);
	free(inputData);
	free(controlSig);
	
	// Disable/destroy timer
	timer_delete(sampling_timer);
	printf("Bye\n");
	return 0;
}




