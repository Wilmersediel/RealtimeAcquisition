#include <stdio.h>
#include <string.h>
#include <SaveData.h>


int SaveData(char * filename, 
			 double *timeArray,
			 double *dataArray, 
			 double *ctrlSignal, 
			 int dataLength)
{
	
	FILE *fp;
	int i;
	
	fp = fopen(filename, "w");	
	
	if(fp == NULL) //Something goes wrong
		return 1;
		
	/*Writes plant data into file*/
	/*The format is: timeArray - ctrlSignal - dataArray*/	
	for(i = 0; i < dataLength; i++)
		fprintf(fp,"%f\t%f\t%f\n",timeArray[i], ctrlSignal[i], dataArray[i]);	
		
	fclose(fp);
	return 0;
}			 






