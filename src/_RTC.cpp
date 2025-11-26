#include <M5Unified.h>
#include <WiFi.h>
#include "TimeLib.h"
#include "time.h"

#include "_RTC.h"

// gps may require cold start assistance from
// the RTC. So we put this routine in the GPS file.


//-----------------------------------------------------------------
char *pDateTimeString() 
{ 
	// format time based on internal ESP 'now' value.
	static char tstring[40];
	
	sprintf(tstring,"%02d/%02d/%4d %02d:%02d:%02d", 
					day(), month(), year(), 
					hour(), minute(), second());
	return tstring;
}

//-----------------------------------------------------------------

uint32_t _getUTCfromRTC(void)
{

	time_t UTC; 		// a time stamp
	
	// cold read the RTC. Did the time stick?
	m5::rtc_time_t TimeStruct;
	m5::rtc_date_t DateStruct;
	tmElements_t tmpTime;	//Time elements structure
	
	M5.Rtc.getTime(&TimeStruct);
	M5.Rtc.getDate(&DateStruct);
	
	Serial.printf("COLD READ RTC time %02d:%02d:%02d\n",
	TimeStruct.hours,
	TimeStruct.minutes,
	TimeStruct.seconds);
	
	Serial.printf("COLD READ RTC date %02d/%02d/%04d\n",
	DateStruct.date,
	DateStruct.month,
	DateStruct.year);

	tmpTime.Hour = TimeStruct.hours;
	tmpTime.Minute = TimeStruct.minutes;
	tmpTime.Second = TimeStruct.seconds;
	
	tmpTime.Day = DateStruct.date;
	tmpTime.Month = DateStruct.month;
	
	// time starts from 1970 ... aka year 0
	tmpTime.Year = DateStruct.year - 1970;

	//thank god I dont have to calc seconds in month 
	UTC =  makeTime(tmpTime);
	
	Serial.printf("calculated UTC from RTC = %d\n", UTC);
	return UTC;
	
}
//---------------------------------------------------------------------------
static void setRtcHW(uint8_t hr, uint8_t min, uint8_t sec, uint8_t day, uint8_t month, uint16_t year)
{
		// set RTC		
		m5::rtc_time_t TimeStruct;
		m5::rtc_date_t DateStruct;

		// stuff into RTC chip
		TimeStruct.hours = hr;
		TimeStruct.minutes = min;
		TimeStruct.seconds = sec;
		
		DateStruct.year = year;
		DateStruct.month = month;
		DateStruct.date = day;
		
		M5.Rtc.setTime(&TimeStruct);
		M5.Rtc.setDate(&DateStruct);
}

//-------------------------------------------------------------
void _setup_RTC(void)
{
    const char* ntpServer = "pool.ntp.org";

    // just get UTC time.
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 0;

	if ( WiFi.status() == WL_CONNECTED)
	{
		configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
		struct tm lcl_tm;

		// now go off to the server and get the NTP time
		if(!getLocalTime(&lcl_tm))
		{
	  		Serial.println("Failed to obtain time");
	  		return;
		}

		Serial.println(&lcl_tm, "NTP PASS: %A, %B %d %Y %H:%M:%S");

		time_t t = mktime(&lcl_tm);

		// RTC contains UTC time.
		setRtcHW(hour(t), minute(t), second(t),
		day(t), month(t), year(t));
		

	}

	// now NTP could have failed. Recover from the RTC clock chip.
	// if  NTP succeeded, it programmed the RTC earlier :) .
	
	// get RTC hw and plug into system clock 'now'
	
	m5::rtc_time_t TimeStruct;
	m5::rtc_date_t DateStruct;

	M5.Rtc.getTime(&TimeStruct);
	M5.Rtc.getDate(&DateStruct);

	// time to smash the system 'now' time
	setTime(TimeStruct.hours,TimeStruct.minutes, TimeStruct.seconds,
			DateStruct.date, DateStruct.month, DateStruct.year);
			
	// adjust system 'now' time for timezone and savings		
	adjustTime ( (- 5 * 3600) + (0 * 3600)) ; //timezone and daylight savings
	
}

