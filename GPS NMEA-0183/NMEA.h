/**********************************************************************//**
@file    nmea.h
@author  Thoap Tang
@date    2016/08/22
@brief   assert 
**************************************************************************/
#ifndef _NMEA_H_
#define _NMEA_H_

///to assert buffer with size 1024
#define BUFFER_SIZE_ONE_TIME 1024
///max satellite ID
#define MAX_SATE_ID 256 
///max pointer number in array of pointers
#define MAX_POINTER_NUMBER 48
///to assert output buffer
#define MAX_OUTPUT_LINE_LENGTH 128
///assert for calculate running time
#define RUNNING_TIME(a, b) ((b) - (a))

///BOOL Flag, TRUE(1) or FALSE(0)
typedef int BOOL;
#define TRUE 1
#define FALSE 0

///date information
typedef struct
{
	unsigned int Day;
	unsigned int Month;
	unsigned int Year;
}NMEA_DATE_T;

///UTC time
typedef struct
{
	unsigned int Hour;
	unsigned int Minute;
	unsigned int Second;
	unsigned int Millisec;
}NMEA_TIME_T;

///Latitude and Longitude
typedef struct
{
	int Degree;
	unsigned int Minute;
	unsigned int Second;
	unsigned int MiliSec;
}NMEA_TUDE_T;

///Data of SV(Satellites in View)
typedef struct
{
	unsigned int SateSys;
	unsigned int SatelliteID;
	unsigned int Elevation;
	unsigned int Azimuth;
	unsigned int SNR;
	BOOL Used;
}NMEA_SV_T;

///Data of course and speed
typedef struct
{
	float SpeedOverGnd;					//speed over ground(knots)
	float CourseOverGnd;				//course over ground(degrees)
	float TrackDegInTrue;				//track degree relate to true north
	float TrackDegInMagnetic;			//track degree relate to magnetic north
	float SpeedN;						//measured horizontal speed(knots)
	float SpeedK;						//measured horizontal speed(km/h)
}NMEA_MOVE_T;

///Data of NMEA
typedef struct
{
	NMEA_DATE_T NMEADate;
	NMEA_TIME_T NMEATime;
	NMEA_TUDE_T Latitude;
	NMEA_TUDE_T Longitude;
	BOOL DataStatus;
	unsigned int PosFixInd;				//Position Fix Indicator
	unsigned int FixMode;
	unsigned int SateNumUsed;			//number of satellites in use
	unsigned int SateInView;			//number of satellites in view
	float Altitude;
	float GeoidSep;						//Geoid Separation
	float PDOP;
	float HDOP;
	float VDOP;
	NMEA_MOVE_T NMEAMove;
	NMEA_SV_T NMEASVData[MAX_POINTER_NUMBER];
	char SateID[MAX_SATE_ID];
}NMEA_DATA_T;

///get a line and get character count
typedef struct
{
	char NMEALine[BUFFER_SIZE_ONE_TIME];
	unsigned int CharacterCount;
}NMEA_LINE_T;

///analysis a line and get pointer count
typedef struct
{
	char *pNMEAnalysis[MAX_POINTER_NUMBER];
	unsigned int PointerCount;
}NMEA_ANALYZER_T;

///reserve output info and get size count
typedef struct
{
	char NMEAOutput[MAX_OUTPUT_LINE_LENGTH];
	unsigned int SizeCount;
}NMEA_OUT_T;

///flag for systems, GPS GLA GAL BDS
typedef struct
{
	BOOL FlagGP;
	BOOL FlagGL;
	BOOL FlagGA;
	BOOL FlagBD;
}FLAG_SYSTEM_T;

///set flag to sentences and reserve last line head info
typedef struct
{
	BOOL FlagGGA;
	BOOL FlagGLL;
	BOOL FlagRMC;
	BOOL FlagVTG;
	BOOL FlagZDA;
	BOOL FlagGSA;
	FLAG_SYSTEM_T FlagSys;
	unsigned int LastLine;	/*None:0 GGA:1 GLL:2 RMC:3 VTG:4 ZDA:5 GSA:6 GSV:7*/
	unsigned int VTGCount;
}NMEA_FLAG_T;

///print error information and exit
void OpenError(void);

///prepare the chart in output file
void ChartPreparer(FILE *fpNMEAData);

///output info and initialize struct NMEAData
void NMEAOutput(NMEA_DATA_T *pNMEAData, FILE *fpNMEAData);

///main of parser, identify Header, parse NMEA data
BOOL NMEAParser(NMEA_ANALYZER_T *NMEAnlyz, NMEA_DATA_T *NMEAData, FILE *fpNMEAData);

/// a frame connect every parts together
void NMEAParseFrame(FILE *fpInput, FILE *fpNMEAData);

#endif