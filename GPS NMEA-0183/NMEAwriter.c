/**********************************************************************//**
@file    NMEAwriter.c
@author  Thoap Tang
@date    2016/08/25
@brief   output Info
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmea.h"

///satellites systems
const char SateSystem[5][6] = { "error", "GPS", "GLN", "GAL", "BDS" };
///Position Fix Indicator
const char FixInd[6][10] = { "Invalid", "GPS", "DGPS", "Invalid", "Invalid", "DR"/* Dead Reckoning */ };
///Fix Mode
const char FixMode[4][10] = { "\0", "Disable", "2D", "3D" };


/**********************************************************************//**
@brief  output Date and UTCTime Info
 
@param  pNMEAData   [In]   struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void DateAndTimePutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T DateTimeData;

	DateTimeData.SizeCount = sprintf(DateTimeData.NMEAOutput, "%02d.%02d.%02d\t%02d:%02d:%02d.%-03d\t",
		pNMEAData->NMEADate.Day, pNMEAData->NMEADate.Month,
		pNMEAData->NMEADate.Year, pNMEAData->NMEATime.Hour,
		pNMEAData->NMEATime.Minute, pNMEAData->NMEATime.Second,
		pNMEAData->NMEATime.Millisec);

	fwrite(DateTimeData.NMEAOutput, 1, DateTimeData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output PDOP, HDOP, VDOP Info
 
@param  pNMEAData   [In]  struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void FixPutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T FixData;

	FixData.SizeCount = sprintf(FixData.NMEAOutput, "%-7s\t%-5s\t",
		FixInd[pNMEAData->PosFixInd], FixMode[pNMEAData->FixMode]);

	fwrite(FixData.NMEAOutput, 1, FixData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output Latitude, Longitude, Altitude and GeoidSep Info
 
@param  pNMEAData   [In]   struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void LLAPutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T LLAData;
	
	LLAData.SizeCount = sprintf(LLAData.NMEAOutput, "%2d¡ã%02d¡ä%02d¡å%-4d\t%3d¡ã%02d¡ä%02d¡å%-4d\t%-7.3f\t%.3f\t",
		pNMEAData->Latitude.Degree, pNMEAData->Latitude.Minute,
		pNMEAData->Latitude.Second, pNMEAData->Latitude.MiliSec,
		pNMEAData->Longitude.Degree, pNMEAData->Longitude.Minute,
		pNMEAData->Longitude.Second, pNMEAData->Longitude.MiliSec,
		pNMEAData->Altitude, pNMEAData->GeoidSep);
	
	fwrite(LLAData.NMEAOutput, 1, LLAData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output PDOP, HDOP, VDOP Info
 
@param  pNMEAData   [In]  struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void DOPPutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T DOPData;

	DOPData.SizeCount = sprintf(DOPData.NMEAOutput, "%.3f\t%.3f\t%.3f\t",
		pNMEAData->PDOP, pNMEAData->HDOP, pNMEAData->VDOP);

	fwrite(DOPData.NMEAOutput, 1, DOPData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output number of sate in view and sate in use
 
@param  pNMEAData   [In]  struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void SateNumPutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T SateNumData;

	SateNumData.SizeCount = sprintf(SateNumData.NMEAOutput, "%02d\t\t%02d\t",
		pNMEAData->SateNumUsed, pNMEAData->SateInView);
	
	fwrite(SateNumData.NMEAOutput, 1, SateNumData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output Course and Speed Info
 
@param  pNMEAData   [In]  struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void MoveDataPutter(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	NMEA_OUT_T MoveData;

	MoveData.SizeCount = sprintf(MoveData.NMEAOutput, "%-6.2f\t%-6.2f\t%-6.2f\t%-6.2f\t%-5.1f\t%-5.1f\t",
		pNMEAData->NMEAMove.SpeedOverGnd, pNMEAData->NMEAMove.CourseOverGnd,
		pNMEAData->NMEAMove.TrackDegInTrue, pNMEAData->NMEAMove.TrackDegInMagnetic,
		pNMEAData->NMEAMove.SpeedN, pNMEAData->NMEAMove.SpeedK);

	fwrite(MoveData.NMEAOutput, 1, MoveData.SizeCount, fpNMEAData);
}


/**********************************************************************//**
@brief  output information in NMEA Data
 
@param  pNMEAData   [In]   struct contains NMEA data information
@param  DataNum     [In]   number of data
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/26
@note 
History:
**************************************************************************/
void NMEADataOutputter(NMEA_DATA_T *pNMEAData, unsigned int DataNum, FILE *fpNMEAData)
{
	fprintf(fpNMEAData, "%-7d\t", DataNum);

	DateAndTimePutter(pNMEAData, fpNMEAData);

	FixPutter(pNMEAData, fpNMEAData);

	LLAPutter(pNMEAData, fpNMEAData);

	DOPPutter(pNMEAData, fpNMEAData);

	SateNumPutter(pNMEAData, fpNMEAData);

	MoveDataPutter(pNMEAData, fpNMEAData);

	fprintf(fpNMEAData, "\r\n");
}


/**********************************************************************//**
@brief  output SV Chart head
 
@param  fpSVID  [Out]  SV Data output file

@author Thoap Tang
@date   2016/08/31
@note 
History:
**************************************************************************/
void ChartPutter(FILE *fpSVID)
{
	fprintf(fpSVID, "%s", "InfoNo.\tSateID\tSystem\tElev\tAzim\tSNR\tUsed\r\n");
}


/**********************************************************************//**
@brief  output one Sate in view Info
 
@param  DataNum   [In]   number of data
@param  pSVData   [In]   pointers for SV Data

@author Thoap Tang
@date   2016/08/31
@note 
History:
**************************************************************************/
void SVOut(unsigned int DataNum, NMEA_SV_T *pSVData, FILE *fpSVID)
{
	NMEA_OUT_T SVData;

	SVData.SizeCount = sprintf(SVData.NMEAOutput, "%-7d\t%-3d\t\t\t%s\t\t\t%-2d\t\t%-3d\t\t%-2d\t%d\r\n", DataNum,
		pSVData->SatelliteID, SateSystem[pSVData->SateSys], pSVData->Elevation,
		pSVData->Azimuth, pSVData->SNR, pSVData->Used);

	fwrite(SVData.NMEAOutput, 1, SVData.SizeCount, fpSVID);
}


/**********************************************************************//**
@brief  output information in NMEA Data
		2nd way:one Sate info in one file
 
@param  DataNum   [In]   number of data
@param  pSVData   [In]   struct contains data of sate in view

@author Thoap Tang
@date   2016/08/31
@note 
History:
**************************************************************************/
void SVDataPutter(unsigned int DataNum, NMEA_SV_T *pSVData)
{
	FILE *fpSVID;
	char SVID[32] = { '\0' };

	static char SateId[MAX_SATE_ID] = { '\0' };			//to check if the file is established
	static char *pSateId[MAX_SATE_ID];				//reserve pointers of files
	
	if (SateId[pSVData->SatelliteID] == 0)
	{
		sprintf(SVID, "./parserInfo/Sate%d.log", pSVData->SatelliteID);

		fpSVID = fopen(SVID, "wb");

		ChartPutter(fpSVID);
		SVOut(DataNum, pSVData, fpSVID);			//out put info

		SateId[pSVData->SatelliteID] = 1;
		pSateId[pSVData->SatelliteID] = (char*)fpSVID;		//reserve the pointer of file
	}

	else if (SateId[pSVData->SatelliteID] != 0)
	{
		fpSVID = (FILE*)pSateId[pSVData->SatelliteID];

		SVOut(DataNum, pSVData, fpSVID);
	}
}


/**********************************************************************//**
@brief  output information in SV Data
 
@param  pNMEAData [In]   struct contains NMEA data information
@param  DataNum   [In]   number of data

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void SVDataOutputter(NMEA_DATA_T *pNMEAData, unsigned int DataNum)
{
	NMEA_SV_T *pSVData;
	unsigned int Bit;

	for (Bit = 0; Bit < pNMEAData->SateInView; Bit += 1)
	{
		pSVData = &pNMEAData->NMEASVData[Bit];

		SVDataPutter(DataNum, pSVData);
	}
}


/**********************************************************************//**
@brief  output info and initialize struct NMEAData
 
@param  pNMEAData   [In]   struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void NMEAOutput(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	static unsigned int s_DataNum = 1;

	NMEADataOutputter(pNMEAData, s_DataNum, fpNMEAData);		//out put NMEA data
	SVDataOutputter(pNMEAData, s_DataNum);				//out put sates in view data

	memset(pNMEAData, 0, sizeof(NMEA_DATA_T));

	s_DataNum += 1;
}
