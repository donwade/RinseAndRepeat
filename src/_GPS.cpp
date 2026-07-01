#include "_m5Core2-only.h"

#include <_TZ.h>
//---------------------------
#include <TinyGPS++.h>
#include <MultipleSatellite.h>
#include <_GPS.h>

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

    Serial.printf("waiting for first fix from GPS\n");

}


static struct kitchenSink_s
{
    double		doubleLat, doubleLng;
    uint32_t	ageFix;
    uint32_t	last;
    double		firstLat, firstLng;
    uint16_t	yearNow;
    uint8_t		monthNow;
    uint8_t		dayNow;
    uint8_t		hourNow;
    uint8_t		minuteNow;
    uint8_t		secondNow;
    double		distanceToFirstFix;
    double		courseToFirstFix;
    uint32_t	epochTimeNow;
    uint8_t		numSats;
    float		kmph;
    float		deg;
    float		altitudeM;
    float		hdop;
} kitchenSink;


bool bFirstFix = false;


#define CalendarYrToTm(Y) ((Y)-1970)
uint32_t getEpochTimeFromGPS()
{
    // 1. Extract individual pieces of date/time from TinyGPS++
    kitchenSink.yearNow = gpsM5.date.year();
    kitchenSink.monthNow = gpsM5.date.month();
    kitchenSink.dayNow = gpsM5.date.day();
    kitchenSink.hourNow = gpsM5.time.hour();
    kitchenSink.minuteNow = gpsM5.time.minute();
    kitchenSink.secondNow = gpsM5.time.second();

    // 2. Format into the standard tmElements_t structure
    // Note: Weekday can be calculated or set to 0 as a dummy value

    tmElements_t tm;

    tm.Year = CalendarYrToTm(kitchenSink.yearNow);     // Converts e.g., 2026 to 56
    tm.Month = kitchenSink.monthNow;
    tm.Day = kitchenSink.dayNow;
    tm.Hour = kitchenSink.hourNow;
    tm.Minute = kitchenSink.minuteNow;
    tm.Second = kitchenSink.secondNow;

    // 3. Convert to UNIX epoch time (seconds since 1/1/1970)
    kitchenSink.epochTimeNow = (uint32_t)makeTime(tm);

    Serial.print("Current Epoch Time: ");
    Serial.println(kitchenSink.epochTimeNow);

    return kitchenSink.epochTimeNow;

}


#define RATE 1000
static uint32_t slowdown = millis() + RATE;
void _refresh_GPS(void)
{
    RawDegrees rawLat;
    RawDegrees rawLng;
    uint32_t ageFix;
    uint32_t safety;


    uint8_t atLine = 1;

    while (millis() < slowdown)
        delay(100);

    ;                                        // dont ask faster than allow or no data.
    slowdown = millis() + RATE;

    gpsM5.updateGPS();


    // Dispatch incoming characters
    //while (gpsM5.available() > 0)
    //  gpsM5.encode(gpsM5.read());

    // epoch is always sent from first found gpsM5.
    uint32_t epTime = getEpochTimeFromGPS();

    ageFix = gpsM5.location.age();

    if (gpsM5.location.isUpdated())
    {
        rawLat = gpsM5.location.rawLat();
        kitchenSink.doubleLat = (double)(rawLat.negative ? -rawLat.deg:rawLat.deg) + rawLat.billionths / 1000000000.;

        rawLng = gpsM5.location.rawLng();
        kitchenSink.doubleLng = (double)(rawLng.negative ? -rawLng.deg:rawLng.deg) + rawLng.billionths / 1000000000.;

        if (!bFirstFix)
        {
            kitchenSink.firstLat = gpsM5.location.lat();
            kitchenSink.firstLng = gpsM5.location.lng();
            bFirstFix = true;
        }
    }
    else if (gpsM5.date.isUpdated())
    {
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
        kitchenSink.kmph = gpsM5.speed.kmph();
        Serial.println(kitchenSink.kmph);
    }
    else if (gpsM5.course.isUpdated())
    {
        kitchenSink.deg = gpsM5.course.deg();
    }
    else if (gpsM5.altitude.isUpdated())
    {
        kitchenSink.altitudeM = gpsM5.altitude.meters();
    }
    else if (gpsM5.satellites.isUpdated())
    {
        kitchenSink.numSats = gpsM5.satellites.value();
    }
    else if (gpsM5.hdop.isUpdated())
    {
        kitchenSink.hdop = gpsM5.hdop.hdop();
    }
    else if (millis() - safety > 1000)
    {
        safety = millis();

        if (gpsM5.charsProcessed() < 10)
            Serial.println(F("WARNING: No GPS data.  Check wiring."));
    }
}


void _test_GPS(void)
{
    RawDegrees rawLat;
    RawDegrees rawLng;
    uint32_t ageFix;
    uint32_t last;


    uint8_t atLine = 1;

    while (millis() < slowdown)
        delay(100);

    ;                                        // dont ask faster than allow or no data.
    slowdown = millis() + RATE;

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
        Serial.printf("Fix Age= %d\n", gpsM5.location.age());

        rawLat = gpsM5.location.rawLat();
        kitchenSink.doubleLat = (double)(rawLat.negative ? -rawLat.deg:rawLat.deg) + rawLat.billionths / 1000000000.;

        Serial.printf("    latD: %+15.10lf short= %f\n",
                      kitchenSink.doubleLat,
                      gpsM5.location.lat());



        rawLng = gpsM5.location.rawLng();
        kitchenSink.doubleLng = (double)(rawLng.negative ? -rawLng.deg:rawLng.deg) + rawLng.billionths / 1000000000.;

        Serial.printf("    lngD: %+15.10lf short= %f\n",
                      kitchenSink.doubleLng,
                      gpsM5.location.lng());

        if (!bFirstFix)
        {
            kitchenSink.firstLat = gpsM5.location.lat();
            kitchenSink.firstLng = gpsM5.location.lng();
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
        kitchenSink.kmph = gpsM5.speed.kmph();
        Serial.println(kitchenSink.kmph);
    }
    else if (gpsM5.course.isUpdated())
    {
        Serial.print(F("COURSE     Fix Age="));
        Serial.print(gpsM5.course.age());
        Serial.print(F("ms Raw="));
        Serial.print(gpsM5.course.value());
        Serial.print(F(" Deg="));
        Serial.println(gpsM5.course.deg());
        kitchenSink.deg = gpsM5.course.deg();
        Serial.println(kitchenSink.deg);
    }
    else if (gpsM5.altitude.isUpdated())
    {
        Serial.print(F("ALTITUDE   Fix Age="));
        Serial.print(gpsM5.altitude.age());
        Serial.print(F("ms Raw="));
        Serial.print(gpsM5.altitude.value());
        Serial.print(F(" Meters="));
        kitchenSink.altitudeM = gpsM5.altitude.meters();
        Serial.print(kitchenSink.altitudeM);

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
        kitchenSink.numSats = gpsM5.satellites.value();
        Serial.println(kitchenSink.numSats);
    }
    else if (gpsM5.hdop.isUpdated())
    {
        Serial.print(F("HDOP       Fix Age="));
        Serial.print(gpsM5.hdop.age());
        Serial.print(F("ms raw="));
        Serial.print(gpsM5.hdop.value());
        Serial.print(F(" hdop="));
        kitchenSink.hdop = gpsM5.hdop.hdop();
        Serial.println(kitchenSink.hdop);
    }
    else if (millis() - last > 1000)
    {
        Serial.println();

        if (gpsM5.location.isValid())
        {
            kitchenSink.distanceToFirstFix =
                TinyGPSPlus::distanceBetween(
                    gpsM5.location.lat(),
                    gpsM5.location.lng(),
                    kitchenSink.firstLat,
                    kitchenSink.firstLng);

            kitchenSink.courseToFirstFix =
                TinyGPSPlus::courseTo(
                    gpsM5.location.lat(),
                    gpsM5.location.lng(),
                    kitchenSink.firstLat,
                    kitchenSink.firstLng);

            Serial.print(F("JITTER     Distance="));
            Serial.print(kitchenSink.distanceToFirstFix, 6);
            Serial.print(F(" m Course-to="));
            Serial.print(kitchenSink.courseToFirstFix, 6);
            Serial.print(F(" degrees ["));
            Serial.print(TinyGPSPlus::cardinal(kitchenSink.courseToFirstFix));
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
    }
}


#define SHOW2LCD 1
#if SHOW2LCD
#include "pretty.h"
#include <_viewController.h>
void _GPS2LCD(void)
{
    uint8_t atLine = 1;

    _cprintf(_GREEN, atLine++, "%+lf", kitchenSink.doubleLat);
    _cprintf(_GREEN, atLine++, "%+lf", kitchenSink.doubleLng);

    _cprintf(_GREEN, atLine++, "%s", autoFILENAME("", "ran"));
    _cprintf(_CYAN, atLine++, "epoch=%d", kitchenSink.epochTimeNow);
    _cprintf(_CYAN, atLine++, "%s", getDDMMYY(kitchenSink.epochTimeNow));

    // gps_time_hr/min/sec is NOT available until sat lock
    // use epoch time instead_

    _cprintf(_YELLOW, atLine++, "%s", getHHMMSSapm(kitchenSink.epochTimeNow));

    _cprintf(_YELLOW, atLine++, "#sats %2d", kitchenSink.numSats);
    _cprintf(_RED, atLine++, "END");
}


#else
void _GPS2LCD(void)
{
}


#endif
