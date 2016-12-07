/**********************************************************************//**
@file    main.c
@author  Thoap Tang
@date    2016/08/23
@brief   open files 
**************************************************************************/
#include <stdio.h>
#include <direct.h>

#include "nmea.h"

/**
* build a folder "parserInfo" to contain output logs
* open files and put into framework of parser
* assert a NMEABuffer to read parts of data, get lines from the buffer
* calculate sum of the line compare with checksum in data to check if the line is right
* put line into parser to analysis, put pointers of info into array
* get and reserve data info based on NMEA protocol
* if it's time to output is judged by time info, sentence parsed and flag of sentences
* output the info in NMEAData and logs named by number of satellites
*/
int main()
{
	FILE *fpNMEAInput = NULL, *fpNMEAData = NULL;

	_mkdir("parserInfo");						//build a folder

	fpNMEAInput = fopen("NMEA.log", "rb");				//open input file	
	fpNMEAData = fopen("./parserInfo/NMEAData.txt", "wb");		//open output files

	if (NULL != fpNMEAInput && NULL != fpNMEAData)
	{
		ChartPreparer(fpNMEAData);				//prepare head of chart
		NMEAParseFrame(fpNMEAInput, fpNMEAData);		//main frame of parser
	}
	else
	{
		OpenError();						//display error info
	}

	fclose(fpNMEAInput);
	fclose(fpNMEAData);

	return 0;
}
