#include "_m5Core2-only.h"

#include <_TZ.h>
//---------------------------
#include <TinyGPS++.h>
#include <MultipleSatellite.h>
#include "pretty.h"
#include <_viewController.h>

static const int RXPin = 13, TXPin = 27;
static const uint32_t GPSBaud = 115200;

void displayInfo();

// inherits from tinyGps++
// Create an instance of MultipleSatellite, assuming we use the Serial1
MultipleSatellite gpsM5(Serial1, GPSBaud, SERIAL_8N1, RXPin, TXPin);

void _setup_GPS(void)
{
    Serial.printf("gpsM5 TxPin=%d RxPin=%d Baud=%d\n", TXPin, RXPin, GPSBaud);
    //M5.Power.setExtOutput(false); // cold start gpsM5
    //delay(1000);
    M5.Power.setExtOutput(true);    // start gpsM5
    gpsM5.begin();

    String version = gpsM5.getGNSSVersion();
	Serial.printf("GNSS SW=%s\r\n", version.c_str());
}

bool bFirstFix = false;
double FIRST_LAT = 0.0, FIRST_LNG = 0.0;
double doubleLat, doubleLng;


#define CalendarYrToTm(Y) ((Y) - 1970)

uint16_t yearNow;
uint8_t  monthNow;
uint8_t  dayNow;
uint8_t  hourNow;
uint8_t  minuteNow;
uint8_t  secondNow;
uint32_t epochTimeNow;

uint32_t getEpochTimeFromGPS() 
{
    // 1. Extract individual pieces of date/time from TinyGPS++
    yearNow 	= gpsM5.date.year();
    monthNow 	= gpsM5.date.month();
    dayNow 	= gpsM5.date.day();
    hourNow 	= gpsM5.time.hour();
    minuteNow  = gpsM5.time.minute();
    secondNow  = gpsM5.time.second();

    // 2. Format into the standard tmElements_t structure
    // Note: Weekday can be calculated or set to 0 as a dummy value

    tmElements_t tm;
    
    tm.Year 	= CalendarYrToTm(yearNow); // Converts e.g., 2026 to 56
    tm.Month 	= monthNow;
    tm.Day 		= dayNow;
    tm.Hour 	= hourNow;
    tm.Minute 	= minuteNow;
    tm.Second 	= secondNow;

    // 3. Convert to UNIX epoch time (seconds since 1/1/1970)
    epochTimeNow = (uint32_t) makeTime(tm);

    //Serial.print("Current Epoch Time: ");
    //Serial.println(epochTime);
   
    return epochTimeNow;
    
}


void _loop_GPS(void)
{
  static RawDegrees rawLat;
  static RawDegrees rawLng;
  static uint32_t   ageFix;
  static uint32_t   last;
  
  static uint8_t numSats;

  uint8_t atLine = 1;
  
  gpsM5.updateGPS();

  // Dispatch incoming characters
  //while (gpsM5.available() > 0)
  //  gpsM5.encode(gpsM5.read());

  // epoch is always sent from first found gpsM5.
  uint32_t epTime = getEpochTimeFromGPS();
  
  ageFix = gpsM5.location.age();
  
  if (gpsM5.location.isUpdated())
  {
    Serial.println("---------------------------------------------");
    Serial.printf ("Fix Age= %d\n", gpsM5.location.age());
    
    rawLat = gpsM5.location.rawLat();
	doubleLat = (double)(rawLat.negative ? -rawLat.deg:rawLat.deg) + rawLat.billionths/1000000000.;

    Serial.printf("    latD: %+15.13lf %c%d.%d short= %f\n", 
    			    doubleLat,
    				rawLat.negative ? '-' : '+',
    				rawLat.deg,
    				rawLat.billionths,
    				gpsM5.location.lat());


    
    rawLng= gpsM5.location.rawLng();
    doubleLng = (double)(rawLng.negative ? -rawLng.deg:rawLng.deg) + rawLng.billionths/1000000000.;

    Serial.printf("    lngD: %+15.13lf %c%d.%d short= %f\n",
    				doubleLng,
    				rawLng.negative ? '-' : '+',
    				rawLng.deg,
    				rawLng.billionths,
    				gpsM5.location.lng());
    Serial.println("---------------------------------------------");
    

	if (!bFirstFix)
	{
		FIRST_LAT = gpsM5.location.lat();
		FIRST_LNG = gpsM5.location.lng();
		bFirstFix = true;
	}
  }

  else if (gpsM5.date.isUpdated())
  {
	Serial.print(F("DATE       Fix Age="));
	Serial.print(gpsM5.date.age());
	Serial.print(F("ms Raw="));
	Serial.print(gpsM5.date.value());
	Serial.print(F(" Year="));
	Serial.print(gpsM5.date.year());
	Serial.print(F(" Month="));
	Serial.print(gpsM5.date.month());
	Serial.print(F(" Day="));
	Serial.println(gpsM5.date.day());
  }

  else if (gpsM5.time.isUpdated())
  {
  	// gpsM5.time params are NOT filled in until sat lock.
  	// could take some time. to get these ones.
  	
    Serial.print(F("TIME       Fix Age="));
    Serial.print(gpsM5.time.age());
    Serial.print(F("ms Raw="));
    Serial.print(gpsM5.time.value());
    Serial.print(F(" Hour="));
    Serial.print(gpsM5.time.hour());
    Serial.print(F(" Minute="));
    Serial.print(gpsM5.time.minute());
    Serial.print(F(" Second="));
    Serial.print(gpsM5.time.second());
    Serial.print(F(" Hundredths="));
    Serial.println(gpsM5.time.centisecond());
  }
  else if (gpsM5.speed.isUpdated())
  {
    Serial.print(F("SPEED      Fix Age="));
    Serial.print(gpsM5.speed.age());
    Serial.print(F("ms Raw="));
    Serial.print(gpsM5.speed.value());
    Serial.print(F(" Knots="));
    Serial.print(gpsM5.speed.knots());
    Serial.print(F(" MPH="));
    Serial.print(gpsM5.speed.mph());
    Serial.print(F(" m/s="));
    Serial.print(gpsM5.speed.mps());
    Serial.print(F(" km/h="));
    Serial.println(gpsM5.speed.kmph());
  }

  else if (gpsM5.course.isUpdated())
  {
    Serial.print(F("COURSE     Fix Age="));
    Serial.print(gpsM5.course.age());
    Serial.print(F("ms Raw="));
    Serial.print(gpsM5.course.value());
    Serial.print(F(" Deg="));
    Serial.println(gpsM5.course.deg());
  }

  else if (gpsM5.altitude.isUpdated())
  {
    Serial.print(F("ALTITUDE   Fix Age="));
    Serial.print(gpsM5.altitude.age());
    Serial.print(F("ms Raw="));
    Serial.print(gpsM5.altitude.value());
    Serial.print(F(" Meters="));
    Serial.print(gpsM5.altitude.meters());
    Serial.print(F(" Miles="));
    Serial.print(gpsM5.altitude.miles());
    Serial.print(F(" KM="));
    Serial.print(gpsM5.altitude.kilometers());
    Serial.print(F(" Feet="));
    Serial.println(gpsM5.altitude.feet());
  }

  else if (gpsM5.satellites.isUpdated())
  {
    Serial.print(F("SATELLITES Fix Age="));
    Serial.print(gpsM5.satellites.age());
    Serial.print(F("ms Value="));
    numSats = gpsM5.satellites.value();
    Serial.println(numSats);
  }

  else if (gpsM5.hdop.isUpdated())
  {
    Serial.print(F("HDOP       Fix Age="));
    Serial.print(gpsM5.hdop.age());
    Serial.print(F("ms raw="));
    Serial.print(gpsM5.hdop.value());
    Serial.print(F(" hdop="));
    Serial.println(gpsM5.hdop.hdop());
  }
  else if (millis() - last > 1000)
  {
    Serial.println();
    if (gpsM5.location.isValid())
    {
      double distanceToFirstFix =
        TinyGPSPlus::distanceBetween(
          gpsM5.location.lat(),
          gpsM5.location.lng(),
          FIRST_LAT, 
          FIRST_LNG);
      double courseToFirstFix =
        TinyGPSPlus::courseTo(
          gpsM5.location.lat(),
          gpsM5.location.lng(),
          FIRST_LAT, 
          FIRST_LNG);

      Serial.print(F("JITTER     Distance="));
      Serial.print(distanceToFirstFix, 6);
      Serial.print(F(" m Course-to="));
      Serial.print(courseToFirstFix, 6);
      Serial.print(F(" degrees ["));
      Serial.print(TinyGPSPlus::cardinal(courseToFirstFix));
      Serial.println(F("]"));
    }

    Serial.print(F("DIAGS      Chars="));
    Serial.print(gpsM5.charsProcessed());
    Serial.print(F(" Sentences-with-Fix="));
    Serial.print(gpsM5.sentencesWithFix());
    Serial.print(F(" Failed-checksum="));
    Serial.print(gpsM5.failedChecksum());
    Serial.print(F(" Passed-checksum="));
    Serial.println(gpsM5.passedChecksum());

    if (gpsM5.charsProcessed() < 10)
      Serial.println(F("WARNING: No GPS data.  Check wiring."));

    last = millis();
    Serial.println();

   	_cprintf(_GREEN, atLine++, "%c%d.%d", 
  		rawLat.negative ? '-' : '+',
	    rawLat.deg,
        rawLat.billionths
        );
    _cprintf(_GREEN, atLine++, "%c%d.%d",
		rawLng.negative ? '-' : '+',
  		rawLng.deg,
  		rawLng.billionths
		); 
  
	_cprintf(_GREEN, atLine++, "%s", autoFILENAME("", "ran"));
	_cprintf(_CYAN,  atLine++, "epoch=%d", epTime);
	_cprintf(_CYAN,  atLine++, "%s", getDDMMYY(epTime)); 

	// gps_time_hr/min/sec is NOT available until sat lock
	// use epoch time instead_

	_cprintf(_YELLOW, atLine++, "%s", getHHMMSSapm(epTime));
	
    _cprintf(_YELLOW, atLine++, "#sats %2d age=%d", numSats, ageFix);
    _cprintf(_RED,atLine++,  "END");
   }
}
