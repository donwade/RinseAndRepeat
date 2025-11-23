//strip all "M5" refereces from this file. May rebase to t-beam later

#include <M5Unified.h>
#include <TinyGPS++.h>
#include "_m5Core2-only.h"
#include "_wavePlayer.h"
#include "_viewController.h"

#include "rtc_wdt.h"
#include "esp_debug_helpers.h"

#define LINE Serial.printf("%s:%d\n", __FUNCTION__, __LINE__)
extern void setup_BN880(void);

static portMUX_TYPE my_mutex;

extern void _runDisplayTask(void *not_used);
extern void _runLightBarTask(void *not_used);
extern void _setup_wavePlayer();

#include "_watchdogs.h"

extern TaskHandle_t speak_file(char *waveFilename);

// The TinyGPS++ object
TinyGPSPlus gps;


//=============================================================
/*
	while 'delaying' monitor the incoming gps stream for characters.
	if serial2 chars are found, stuff them into the gps encoder.

	Note: Tdelay, not delay is called.
	      the requested delay could be longer than the watchdog
	      timer, and that is not considered a sin.
	      Tdelay will kick the watchdog as needed if the user
	      requested time is gt dog timer.
	
*/

void smartDelay(unsigned long ms) 
{
	
    unsigned long start = millis();
	
    do {
        while (Serial2.available() > 0)	gps.encode(Serial2.read());
		_Tdelay((8 * 9600/1000)); // 9600baud in mS allow any task dogs !!!!!
    } while (millis() - start < ms);
    //clear();
}

//=============================================================
void displayInfo() {

	_lclear();
	
    _lsetCursor(0, 40);
    _lprint(F("Latitude:    "));
    if (gps.location.isValid()) {
        _lprint(gps.location.lat(), 6);

    } else {
        _lprint(F("INVALID"));
    }

    _lprintln();
    _lprint(F("Longitude:    "));
    if (gps.location.isValid()) {
        _lprint(gps.location.lng(), 6);
    } else {
        _lprint(F("INVALID"));
    }

    _lprintln();
    _lprint(F("Altitude:    "));
    if (gps.altitude.isValid()) {
        _lprint(gps.altitude.meters());
    } else {
        _lprint(F("INVALID"));
    }

    _lprintln();
    _lprint(F("Satellites:    "));
    if (gps.satellites.isValid()) {
        _lprint(gps.satellites.value());
    } else {
        _lprint(F("INVALID"));
    }

    _lprintln();
    _lprint(F("Date: "));
    if (gps.date.isValid()) {
        _lprint(gps.date.month());
        _lprint(F("/"));
        _lprint(gps.date.day());
        _lprint(F("/"));
        _lprint(gps.date.year());
    } else {
        _lprint(F("INVALID"));
    }

    _lprintln();
    _lprint(F("Time: "));
    if (gps.time.isValid()) {
        if (gps.time.hour() < 10) _lprint(F("0"));
        _lprint(gps.time.hour());
        _lprint(F(":"));
        if (gps.time.minute() < 10) _lprint(F("0"));
        _lprint(gps.time.minute());
        _lprint(F(":"));
        if (gps.time.second() < 10) _lprint(F("0"));
        _lprint(gps.time.second());
        _lprint(F("."));
        if (gps.time.centisecond() < 10) _lprint(F("0"));
        _lprint(gps.time.centisecond());
    } else {
        _lprint(F("INVALID"));
    }
}


void _runMenuTask(void *NOTUSED)
{
	_touchPanel_impl();
}

void loop() 
{
	_loop_M5();

//	delay(100);
//	vTaskDelete(NULL);
}



//=============================================================
void setup() {
	Serial.begin(115200);

	while(!Serial) delay(100); // in event of crash loop
	delay(1000);
	for (int j= 0; j++; j < 10) Serial.println();
	Serial.printf("BUILT ON %s %s *** ESP-IDF VER = %s ***\n", __DATE__, __TIME__, esp_get_idf_version());
	delay(3000);

	_setup_M5();	
    _setup_wavePlayer();

	//BN880 takes care of below.
    //Serial2.begin(9600, SERIAL_8N1, 13, 14);

	_lsetTextColor(_WHITE, _BLACK);
    _lsetCursor(0, 0);

	_spawnTaskAndDogV2( _wavPlayerTask, 		//(void * not_used)TaskFunction_t pvTaskCode,
                     "wavPlayerTask",    //const char * const pcName,
                     1024 * 10,		//const uint32_t usStackDepth,
                     NULL,			//void * const pvParameters,
                     4           	//UBaseType_t uxPriority)
                     );
	delay(200);

	_spawnTaskAndDogV2( _runMenuTask, 		//(void * not_used)TaskFunction_t pvTaskCode,
                     "MenuTask",    //const char * const pcName,
                     1024 * 10,		//const uint32_t usStackDepth,
                     NULL,			//void * const pvParameters,
                     4           	//UBaseType_t uxPriority)
                     );

	delay(200);

	_spawnTaskAndDogV2( _runLightBarTask,	//(void * not_used)TaskFunction_t pvTaskCode,
					 "LightBarTask",	//const char * const pcName,
					 1024 * 8,		 	//const uint32_t usStackDepth,
					 NULL,				//void * const pvParameters,
					 4				//UBaseType_t uxPriority)
					 );

	_setToggleColors(_RED, _CYAN, 2);

}

