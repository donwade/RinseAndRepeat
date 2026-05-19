#include "_m5Core2-only.h"

#include <_TZ.h>

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};	// UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};	// UTC - 5 hours

//---------------------------
#include <TinyGPS++.h>
#include <MultipleSatellite.h>

extern MultipleSatellite gpsM5;

//---------------------------
char *getHHMMSS(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	strftime(msg, sizeof(msg), "%I:%M:%S", remote);
	return msg;
}
//---------------------------

char *getHHMM(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	strftime(msg, sizeof(msg), "%I:%M", remote);
	return msg;
}

char *getHHMMSSapm(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	strftime(msg, sizeof(msg), "%I:%M:%S %p", remote);
	return msg;
}


char *getHHMMapm(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	strftime(msg, sizeof(msg), "%I:%M%p", remote);
	return msg;
}

char *getDDMMYY(uint32_t utc)
{
	static char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	//https://www.geeksforgeeks.org/cpp/strftime-function-in-c/

	strftime(msg, sizeof(msg), "%d/%b/%y", remote);
	return msg;
}



char *autoFILENAME(char*prefix, char *ext )
{
	uint32_t utc = getEpochTimeFromGPS();
	static char retval[70];
	char msg[70];
	Timezone usEastern(usEDT, usEST);  // setup dst and timezone recipes

    time_t local = usEastern.toLocal(utc);
    
	struct tm *remote ;
	remote = localtime( &local );

	strftime(msg, sizeof(msg), "%y%m%d", remote);  // all numbers yymmdd

	snprintf(retval, sizeof(retval),  "%s%X.%s", prefix, atoi(msg), ext);
	return retval;
}


