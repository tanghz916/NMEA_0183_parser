/**********************************************************************//**
@file    NMEAparser.c
@author  Thoap Tang
@date    2016/08/24
@brief   parse data
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nmea.h"


/**********************************************************************//**
@brief  read the header, find out which system the info come from
 
@param  pHeader  [In]  head of line

@retval 0:  error
@retval 1:  GPS
@retval 2:  GLN
@retval 3:  GAL
@retval 4:  BDS

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
unsigned int SatelliteSystemReader(char *pHeader)
{
	if (0 == memcmp(pHeader + 1, "GP", 2))		//GPS, Global Positioning System
	{
		return 1;
	}
	else if (0 == memcmp(pHeader + 1, "GL", 2))	//GLN, GLONASS
	{
		return 2;
	}
	else if (0 == memcmp(pHeader + 1, "GA", 2))	//GAL, Galileo satellite navigation system
	{
		return 3;
	}
	else if (0 == memcmp(pHeader + 1, "BD", 2))	//BDS, BeiDou Navigation Satellite System
	{
		return 4;
	}
	else
	{
		return 0;
	}
}


/**********************************************************************//**
@brief  parser UTCTime
 
@param  pTime      [In]   time read from line
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/09/01
@note 
History:
**************************************************************************/
void UTCTimeReader(char *pTime, NMEA_DATA_T *pNMEAData)
{
	double UTCTime;
	unsigned int Time;

	UTCTime = atof(pTime);
	Time = (unsigned int)UTCTime;

	pNMEAData->NMEATime.Hour = Time / 10000;
	pNMEAData->NMEATime.Minute = (Time / 100) % 100;
	pNMEAData->NMEATime.Second = Time % 100;
	pNMEAData->NMEATime.Millisec = (unsigned int)((UTCTime - Time) * 1000);
	
	/*char Hour[] = { pTime[0], pTime[1], '\0' };
	char Minute[] = { pTime[2], pTime[3], '\0' };
	char Second[] = { pTime[4], pTime[5], '\0' };
	char Millisec[] = { pTime[7], pTime[8], pTime[9], '\0' };
	
	pNMEAData->NMEATime.Hour = atoi(Hour);
	pNMEAData->NMEATime.Minute = atoi(Minute);
	pNMEAData->NMEATime.Second = atoi(Second);
	pNMEAData->NMEATime.Millisec = atoi(Millisec);*/
}


/**********************************************************************//**
@brief  parser Date
 
@param  pDate      [In]   date read from line
@param  pNMEAData  [Out]  struct contains NMEA data information

@retval CDate:  struct contains date Info

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void DateReader(char *pDate, NMEA_DATA_T *pNMEAData)
{
	unsigned int Day, Month, Year, Date;

	Date = atoi(pDate);

	Day = Date / 10000;
	Month = (Date / 100) % 100;
	Year = Date % 100;

	pNMEAData->NMEADate.Day = Day;
	pNMEAData->NMEADate.Month = Month;
	pNMEAData->NMEADate.Year = Year;

	/*char Day[] = { pDate[0], pDate[1], '\0' };
	char Month[] = { pDate[2], pDate[3], '\0' };
	char Year[] = { pDate[4], pDate[5], '\0' };

	pNMEAData->NMEADate.Day = atoi(Day);
	pNMEAData->NMEADate.Month = atoi(Month);
	pNMEAData->NMEADate.Year = atoi(Year);*/
}


/**********************************************************************//**
@brief  parser Latitude and Longitude
 
@param  pLatitude  [In]  Latitude read from line
@param  pInd	   [In]  Ind of Latitude

@retval NMEAtude:  struct contains Lat or Lon Info

@author Thoap Tang
@date   2016/09/01
@note 
History:
**************************************************************************/
NMEA_TUDE_T TudeReader(char *pTude, char *pInd)
{
	NMEA_TUDE_T NMEATude;
	double Tude;
	unsigned int nTude;

	Tude = atof(pTude);
	nTude = (unsigned int)Tude;

	NMEATude.Degree = nTude / 100;
	NMEATude.Minute = nTude % 100;
	NMEATude.Second = (unsigned int)((Tude - nTude) * 100);
	NMEATude.MiliSec = (unsigned int)(Tude * 1000000) % 10000;

	if (0 == strcmp(pInd, "S") || 0 == strcmp(pInd, "W"))
	{
		NMEATude.Degree = -NMEATude.Degree;
	}

	return NMEATude;
}


/**********************************************************************//**
@brief  parser GGA information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/24
@note 
History:
**************************************************************************/
void GGAParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	char *pPosFixInd=pNMEAnlyz->pNMEAnalysis[6];
	
	if (strcmp(pPosFixInd, "1") == 0 || strcmp(pPosFixInd, "2") == 0 || strcmp(pPosFixInd, "6") == 0)
	{
		UTCTimeReader(pNMEAnlyz->pNMEAnalysis[1], pNMEAData);

		pNMEAData->Latitude = TudeReader(pNMEAnlyz->pNMEAnalysis[2], pNMEAnlyz->pNMEAnalysis[3]);
		pNMEAData->Longitude = TudeReader(pNMEAnlyz->pNMEAnalysis[4], pNMEAnlyz->pNMEAnalysis[5]);

		pNMEAData->PosFixInd = atoi(pPosFixInd);

		pNMEAData->SateNumUsed = atoi(pNMEAnlyz->pNMEAnalysis[7]);

		pNMEAData->HDOP = (float)atof(pNMEAnlyz->pNMEAnalysis[8]);

		pNMEAData->Altitude = (float)atof(pNMEAnlyz->pNMEAnalysis[9]);
		pNMEAData->GeoidSep = (float)atof(pNMEAnlyz->pNMEAnalysis[11]);
		
	}
}


/**********************************************************************//**
@brief  parser GLL information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/24
@note 
History:
**************************************************************************/
void GLLParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	if (strcmp(pNMEAnlyz->pNMEAnalysis[6], "A") == 0)
	{
		pNMEAData->Latitude = TudeReader(pNMEAnlyz->pNMEAnalysis[1], pNMEAnlyz->pNMEAnalysis[2]);
		pNMEAData->Longitude = TudeReader(pNMEAnlyz->pNMEAnalysis[3], pNMEAnlyz->pNMEAnalysis[4]);

		UTCTimeReader(pNMEAnlyz->pNMEAnalysis[5], pNMEAData);
	}
}


/**********************************************************************//**
@brief  parser GSA information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void GSAParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	unsigned int Channel, SateID;

	char * pFixMode= pNMEAnlyz->pNMEAnalysis[2];

	if (strcmp(pFixMode, "2") == 0 || strcmp(pFixMode, "3") == 0)
	{
		for (Channel = 1; Channel < 13; Channel += 1)
		{
			SateID = atoi(pNMEAnlyz->pNMEAnalysis[Channel + 2]);
			pNMEAData->SateID[SateID] = 1;		//to check if the sate in view is used 
		}

		pNMEAData->FixMode = atoi(pFixMode);

		pNMEAData->PDOP = (float)atof(pNMEAnlyz->pNMEAnalysis[15]);
		pNMEAData->HDOP = (float)atof(pNMEAnlyz->pNMEAnalysis[16]);
		pNMEAData->VDOP = (float)atof(pNMEAnlyz->pNMEAnalysis[17]);
	}
}


/**********************************************************************//**
@brief  parser GSV information

@param  pHeader    [In]   head of line
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void GSVParser(char *pHeader, NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	unsigned int Bit , Byte;
	NMEA_SV_T *pSVData;
	unsigned int SateID;
	char SateSys;
	 
	SateSys = SatelliteSystemReader(pHeader);

	Bit = pNMEAData->SateInView;
	
	for (Byte = 4; Byte < pNMEAnlyz->PointerCount; Byte += 4)
	{
		pSVData = &pNMEAData->NMEASVData[Bit];		//used a array of pointer to reserve each sates info

		SateID = atoi(pNMEAnlyz->pNMEAnalysis[Byte]);

		pSVData->SateSys = SateSys;
		pSVData->SatelliteID = SateID;
		pSVData->Elevation = atoi(pNMEAnlyz->pNMEAnalysis[Byte + 1]);
		pSVData->Azimuth = atoi(pNMEAnlyz->pNMEAnalysis[Byte + 2]);
		pSVData->SNR = atoi(pNMEAnlyz->pNMEAnalysis[Byte + 3]);
		pSVData->Used = (pNMEAData->SateID[SateID] == 1);

		Bit += 1;
	}

	pNMEAData->SateInView = Bit;
}


/**********************************************************************//**
@brief  parser RMC information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void RMCParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	char *pStatus = pNMEAnlyz->pNMEAnalysis[2];

	if (strcmp(pStatus, "A") == 0)
	{
		UTCTimeReader(pNMEAnlyz->pNMEAnalysis[1], pNMEAData);

		pNMEAData->Latitude = TudeReader(pNMEAnlyz->pNMEAnalysis[3], pNMEAnlyz->pNMEAnalysis[4]);
		pNMEAData->Longitude = TudeReader(pNMEAnlyz->pNMEAnalysis[5], pNMEAnlyz->pNMEAnalysis[6]);

		pNMEAData->NMEAMove.SpeedOverGnd = (float)atof(pNMEAnlyz->pNMEAnalysis[7]);
		pNMEAData->NMEAMove.CourseOverGnd = (float)atof(pNMEAnlyz->pNMEAnalysis[8]);

		DateReader(pNMEAnlyz->pNMEAnalysis[9], pNMEAData);
	}
}


/**********************************************************************//**
@brief  parser VTG information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void VTGParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	pNMEAData->NMEAMove.TrackDegInTrue = (float)atof(pNMEAnlyz->pNMEAnalysis[1]);

	pNMEAData->NMEAMove.TrackDegInMagnetic = (float)atof(pNMEAnlyz->pNMEAnalysis[3]);

	pNMEAData->NMEAMove.SpeedN = (float)atof(pNMEAnlyz->pNMEAnalysis[5]);

	pNMEAData->NMEAMove.SpeedK = (float)atof(pNMEAnlyz->pNMEAnalysis[7]);
}


/**********************************************************************//**
@brief  parser ZDA information
 
@param  pNMEAnlyz  [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData  [Out]  struct contains NMEA data information

@author Thoap Tang
@date   2016/08/25
@note 
History:
**************************************************************************/
void ZDAParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData)
{
	UTCTimeReader(pNMEAnlyz->pNMEAnalysis[1], pNMEAData);

	pNMEAData->NMEADate.Day = atoi(pNMEAnlyz->pNMEAnalysis[2]);
	pNMEAData->NMEADate.Month = atoi(pNMEAnlyz->pNMEAnalysis[3]);
	pNMEAData->NMEADate.Year = atoi(pNMEAnlyz->pNMEAnalysis[4]);
}


/**********************************************************************//**
@brief  judge if time changed
 
@param  pTimeNow  [In]     time from new line
@param  pTimeRe   [InOut]  time reserved

@retval TRUE:   time didn't change
@retval FALSE:  time changed

@author Thoap Tang
@date   2016/09/02
@note 
History:
**************************************************************************/
BOOL TimeHandler(char *pTimeNow,char *pTimeRe)
{
	if (pTimeRe[0] == '\0')
	{
		return TRUE;
	}
	else if (0 != memcmp(pTimeNow, pTimeRe, 10))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


/**********************************************************************//**
@brief  set flag to system
 
@param  GSVSystem  [In]   system number
@param  Flag       [Out]  flag of system

@author Thoap Tang
@date   2016/09/06
@note 
History:
**************************************************************************/
void SystemFlagSetter(unsigned int GSVSystem, NMEA_FLAG_T *Flag)
{
	switch (GSVSystem)
	{
	case 1:
		Flag->FlagSys.FlagGP = TRUE;
		break;

	case 2:
		Flag->FlagSys.FlagGL = TRUE;
		break;

	case 3:
		Flag->FlagSys.FlagGA = TRUE;
		break;

	case 4:
		Flag->FlagSys.FlagBD = TRUE;
		break;

	default:
		break;
	}
}


/**********************************************************************//**
@brief  main of parser, identify Header, parse NMEA data
 
@param  pNMEAnlyz   [In]   struct include pNMEAnalysis and PointerCount
@param  pNMEAData   [In]   struct contains NMEA data information
@param  fpNMEAData  [Out]  NMEA Data output file

@retval TRUE:   parse success
@retval FALSE:  parse fail

@author Thoap Tang
@date   2016/08/24
@note 
History:
**************************************************************************/
BOOL NMEAParser(NMEA_ANALYZER_T *pNMEAnlyz, NMEA_DATA_T *pNMEAData, FILE *fpNMEAData)
{
	static NMEA_FLAG_T s_Flag;
	static char s_TimeReserve[12] = { '\0' };		//static str to reserve time

	unsigned int GSVSystem = 0;
	char *pHeader;
	
	pHeader = pNMEAnlyz->pNMEAnalysis[0];			//get header to judge sentence type

	if (0 == memcmp(pHeader + 3, "GGA", 3))
	{
		if (0 == TimeHandler(pNMEAnlyz->pNMEAnalysis[1], s_TimeReserve)
			|| (TRUE == s_Flag.FlagGGA && 1 != s_Flag.LastLine))		//judge time and flag, reserve new time if changed
		{																			
			NMEAOutput(pNMEAData, fpNMEAData);				

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		GGAParser(pNMEAnlyz, pNMEAData);         		

		memcpy(s_TimeReserve, pNMEAnlyz->pNMEAnalysis[1], 10);

		s_Flag.LastLine = 1;														//reserve last line message
		s_Flag.FlagGGA = TRUE;														//reserve flag message
	}

	else if (0 == memcmp(pHeader + 3, "GLL", 3))
	{
		if (0 == TimeHandler(pNMEAnlyz->pNMEAnalysis[5], s_TimeReserve)
			|| (TRUE == s_Flag.FlagGLL && 2 != s_Flag.LastLine))
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		GLLParser(pNMEAnlyz, pNMEAData);

		memcpy(s_TimeReserve, pNMEAnlyz->pNMEAnalysis[5], 10);

		s_Flag.LastLine = 2;
		s_Flag.FlagGLL = TRUE;
	}

	else if (0 == memcmp(pHeader + 3, "RMC", 3))
	{
		if (0 == TimeHandler(pNMEAnlyz->pNMEAnalysis[1], s_TimeReserve)
			|| (TRUE == s_Flag.FlagRMC && 3 != s_Flag.LastLine))
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		RMCParser(pNMEAnlyz, pNMEAData);

		memcpy(s_TimeReserve, pNMEAnlyz->pNMEAnalysis[1], 10);

		s_Flag.LastLine = 3;
		s_Flag.FlagRMC = TRUE;
	}

	else if (0 == memcmp(pHeader + 3, "GSA", 3))
	{
		if (TRUE == s_Flag.FlagGSA && 6 != s_Flag.LastLine)
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		GSAParser(pNMEAnlyz, pNMEAData);

		s_Flag.LastLine = 6;
		s_Flag.FlagGSA = TRUE;
	}

	else if (0 == memcmp(pHeader + 3, "GSV", 3))
	{
		GSVSystem = SatelliteSystemReader(pHeader);			

		if ((1 == GSVSystem && TRUE == s_Flag.FlagSys.FlagGP) ||		//check system and flag		
			(2 == GSVSystem && TRUE == s_Flag.FlagSys.FlagGL) ||
			(3 == GSVSystem && TRUE == s_Flag.FlagSys.FlagGA) ||
			(4 == GSVSystem && TRUE == s_Flag.FlagSys.FlagBD))
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		GSVParser(pHeader, pNMEAnlyz, pNMEAData);

		if (*pNMEAnlyz->pNMEAnalysis[1] == *pNMEAnlyz->pNMEAnalysis[2])		//check the number of sentence
		{
			SystemFlagSetter(GSVSystem, &s_Flag);				//set flag to system
		}

		s_Flag.LastLine = 7;
	}

	else if (0 == memcmp(pHeader + 3, "VTG", 3))
	{
		if (TRUE == s_Flag.FlagVTG && 4 != s_Flag.LastLine || 3 <= s_Flag.VTGCount)
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		VTGParser(pNMEAnlyz, pNMEAData);

		s_Flag.VTGCount += 1;
		s_Flag.LastLine = 4;
		s_Flag.FlagVTG = TRUE;
	}

	else if (0 == memcmp(pHeader + 3, "ZDA", 3))
	{
		if (0 == TimeHandler(pNMEAnlyz->pNMEAnalysis[1], s_TimeReserve)
			|| (TRUE == s_Flag.FlagZDA && 5 != s_Flag.LastLine))
		{
			NMEAOutput(pNMEAData, fpNMEAData);

			memset(s_TimeReserve, 0, sizeof(s_TimeReserve));
			memset(&s_Flag, FALSE, sizeof(NMEA_FLAG_T));
		}

		ZDAParser(pNMEAnlyz, pNMEAData);

		memcpy(s_TimeReserve, pNMEAnlyz->pNMEAnalysis[1], 10);

		s_Flag.LastLine = 5;
		s_Flag.FlagZDA = TRUE;
	}

	else
	{
		return FALSE;
	}

	return TRUE;
}
