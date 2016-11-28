/**********************************************************************//**
@file    NMEAreader.c
@author  Thoap Tang
@date    2016/08/24
@brief   read, check and analysis
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "nmea.h"


/**********************************************************************//**
@brief  print error information and exit

@author Thoap Tang
@date   2016/08/23
@note 
History:
**************************************************************************/
void OpenError(void)
{
	perror("Open error");
	printf("Press enter to exit.");
	getchar();
	exit(1);
}


/**********************************************************************//**
@brief  prepare the chart in output file

@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void ChartPreparer(FILE *fpNMEAData)
{
	/*txt*/
	NMEA_OUT_T NMEAChart;

	NMEAChart.SizeCount = sprintf(NMEAChart.NMEAOutput, "%s",
		"InfoNo.\tDate\t\t\tTime\t\t\t\t\tFInd\t\tFMode\tLatitude\t\t\tLongitude\t\t\
		Alt\t\t\tGSep\tPDOP\tHDOP\tVDOP\tSUse\tSV\tSOG\t\t\tCOG\t\t\tTrkT\t\tTrkM\t\tSpN\t\tSpK\r\n");

	fwrite(NMEAChart.NMEAOutput, 1, NMEAChart.SizeCount, fpNMEAData);
	
	/*excel
	fprintf(fpNMEAData, "%s", "SVData\tDate\tTime\tFInd\tFMode\t");
	fprintf(fpNMEAData, "%s", "Latitude\tLongitude\tAlt\tGSep\tPDOP\tHDOP\tVDOP\t");
	fprintf(fpNMEAData, "%s", "SUse\tSV\tSOG\tCOG\tTrkT\tTrkM\tSpN\tSpK\r\n");
	*/
}


/**********************************************************************//**
@brief  display number of line that parsed success or fail

@param  Success  [In]  number of line that parsed successfully
@param  Fail     [In]  number of line that parsed failed
@param  Start    [In]  time of program start
@param  Start    [In]  time of program end

@author Thoap Tang
@date   2016/08/23
@note 
History:
**************************************************************************/
void RunningInfo(unsigned int Success, unsigned int Fail, unsigned int Start, unsigned int End)
{
	printf("*************\n");
	printf("Success:%d\nFail:%d\n", Success, Fail);	
	printf("*************\n");
	printf("RunTime:%d ms\n", RUNNING_TIME(Start, End));

	system("pause");
}


/**********************************************************************//**
@brief  get Line from NMEABuffer
		calculate count of characters in the Line

@param  Character  [In]  character of one byte in the Line  
@param  pLine     [Out]  struct include NMEALine and CharacterCount

@author Thoap Tang
@date   2016/08/23
@note 
History:
**************************************************************************/
void NMEALineGetter(char Character, NMEA_LINE_T *pLine)
{
	if (Character == '$')
	{
		memset(pLine, 0, sizeof(NMEA_LINE_T));
	}

	pLine->NMEALine[pLine->CharacterCount] = Character;
	pLine->CharacterCount += 1;
}


/**********************************************************************//**
@brief  calculate checksum of line

@param  Check  [In]  pointer of checksum calculate start

@retval Checksum: checksum calculated

@author Thoap Tang
@date   2016/08/30
@note 
History:
**************************************************************************/
unsigned int ChecksumCalculater(char* Check)
{
	char Checksum = 0x00;
	unsigned int Length, Bit = 0;

	Length = strlen(Check);			//get length

	while (Bit < Length)
	{
		Checksum ^= Check[Bit++];	//calculate checksum
	}
	return Checksum;
}


/**********************************************************************//**
@brief  calculate checksum of line
		compare with checksum get from data
 
@param  pNMEALine  [In]  struct include NMEALine and CharacterCount

@retval TRUE:   checksum is right
@retval FALSE:  checksum is wrong

@author Thoap Tang
@date   2016/08/30
@note 
History:
**************************************************************************/
BOOL MNEAChecksumChecker(NMEA_LINE_T *pNMEALine)
{
	char *pCheckBegin = NULL, *pCheckEnd = NULL;
	unsigned int LineChecksum = 0x00, CalChecksum = 0x00;
	
	pCheckBegin = strchr(pNMEALine->NMEALine, '$');		//where checksum calculate start
	pCheckEnd = strchr(pNMEALine->NMEALine, '*');		//where checksum calculate end
	
	if (NULL == pCheckBegin || NULL == pCheckEnd)		//prevent project from break cuz by it come to end of buffer and the pointer is NULL
	{
		return FALSE;													
	}

	*pCheckEnd = 0;

	CalChecksum = ChecksumCalculater(pCheckBegin + 1);	//calculate checksum
	LineChecksum = strtol(pCheckEnd + 1, NULL, 16);		//get checksum from line

	return (CalChecksum == LineChecksum) ? TRUE : FALSE;	//compare checksum
}


/**********************************************************************//**
@brief  analyze a line, put line into array of pointers
		and count the number of pointers
 
@param  pNMEALine  [In]   struct include NMEALine and CharacterCount
@param  pNMEAnlyz  [Out]  struct include pNMEAnalysis and PointerCount

@retval TRUE:   checksum is right
@retval FALSE:  checksum is wrong

@author Thoap Tang
@date   2016/08/24
@note 
History:
**************************************************************************/
void NMEAAnalysiser(NMEA_LINE_T *pNMEALine, NMEA_ANALYZER_T *pNMEAnlyz)
{
	char *Pointer;
	char Character;
	unsigned int Byte;
	unsigned int GetPointer = TRUE;
	unsigned int StopGetPointer = FALSE;
	BOOL FlagPointer;

	FlagPointer = GetPointer;

	memset(pNMEAnlyz, 0, sizeof(NMEA_ANALYZER_T));		//initialize struct
	
	for (Byte = 0; Byte < pNMEALine->CharacterCount; Byte += 1)
	{
		if (TRUE == FlagPointer)
		{
			Pointer = &pNMEALine->NMEALine[Byte];	//get pointer of line

			pNMEAnlyz->pNMEAnalysis[pNMEAnlyz->PointerCount] = Pointer;	//put pointers into array
			pNMEAnlyz->PointerCount += 1;

			FlagPointer = StopGetPointer;		//set flag to stop get pointer
		}

		Character = pNMEALine->NMEALine[Byte];

		if (Character == ',')
		{
			pNMEALine->NMEALine[Byte] = 0;		//set 0 to cut when there is a ','

			FlagPointer = GetPointer;		//set flag to get pointer
		}
	}
}


/**********************************************************************//**
@brief  a frame connect every parts together
 
@param  fpInput     [In]   input file, information source
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/24
@note 
History:
**************************************************************************/
void NMEAParseFrame(FILE *fpInput, FILE *fpNMEAData)
{
	NMEA_DATA_T NMEAData;
	NMEA_LINE_T NMEALine;
	NMEA_ANALYZER_T NMEAnlyz;

	char NMEABuffer[BUFFER_SIZE_ONE_TIME] = { '\0' };
	char Character;
	unsigned int BufferCount, LineByte;
	unsigned int NumberSuccess = 0, NumberFail = 0;
	unsigned int StartTime, EndTime;

	StartTime = clock();

	memset(&NMEAData, 0, sizeof(NMEA_DATA_T));
	memset(&NMEALine, 0, sizeof(NMEA_LINE_T));				//initialize struct

	BufferCount = fread(NMEABuffer, 1, sizeof(NMEABuffer), fpInput);	//put part of Info into buffer, and get size

	while (BufferCount > 0)
	{
		for (LineByte = 0; LineByte < BufferCount; LineByte += 1)
		{
			Character = NMEABuffer[LineByte];											//get Character for buffer

			NMEALineGetter(Character, &NMEALine);			//get the line into CNMEALine->NMEALine

			if (Character == '\n')
			{
				if (MNEAChecksumChecker(&NMEALine))											//check checksum
				{
					NMEAAnalysiser(&NMEALine, &NMEAnlyz);	//put line into array of pointers

					if (NMEAParser(&NMEAnlyz, &NMEAData, fpNMEAData)) 	
					{
						NumberSuccess++;
					}
					else
					{
						NumberFail++;
					}
				}
				else
				{
					NumberFail += 1;
				}
			}
		}
		BufferCount = fread(NMEABuffer, 1, sizeof(NMEABuffer), fpInput);
	}
	
	EndTime = clock();

	RunningInfo(NumberSuccess, NumberFail, StartTime, EndTime);			//display running time and number of success or fail 
}
