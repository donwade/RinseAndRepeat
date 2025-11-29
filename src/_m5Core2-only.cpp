#include <M5Unified.h>
#include "_m5Core2-only.h"
#include "_viewController.h"
#include "_OTAUpload.h"
#include "_RTC.h"

#include "esp_debug_helpers.h" // not used here but is useful
/* 
reference?
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT
*/

static uint8_t vert = 0;

void _lsetCursor(uint16_t X, uint16_t Y)
{
	//int16_t horz =M5.Lcd.textWidth(" ", font);
	//printf("charX=%d\n", horz);

	if (!vert) vert = M5.Lcd.fontHeight() + 3;
	//printf("charY=%d\n", vert);
	
	M5.Lcd.setCursor(X, Y * vert);
}

void _lprint(const __FlashStringHelper *x)
{
	M5.Lcd.print(x);
}

void _lprint(char *x)
{
	M5.Lcd.print(x);
}

void _lprint(uint8_t x)
{
	M5.Lcd.print(x);
}


void _lprint(double x, int y)
{
	M5.Lcd.print(x,y);
}

void _lprintln(void)
{
	M5.Lcd.println();
}

void _lprintln(char *x)
{
	M5.Lcd.println(x);
}

void _lclear(void)
{
	M5.Lcd.clear();
}
//--------------------------------------------------
#include <Adafruit_GFX.h>    // Core graphics library
// below must follow above
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeMono12pt7b.h>

//--------------------------------------------------
static const gpio_num_t SDCARD_CSPIN = GPIO_NUM_4;
#include <SD.h>

//-------------------------------------------------------------
void _setup_M5(void)
{
	Serial.begin(115200);

	#if defined(ARDUINO_M5STACK_CORES3)
	
		//https://github.com/donwade/core2S3-bug-uart-issue
		//S3 chip JTAG messes up serial port. Tell it to fuck off
		//If not corrected, 
		//    1) serial input is rejected as a monitor
		//    2) future downloads are rejected and your screwed.
		
		pinMode(19,INPUT);
		#pragma warning("protecting USB serial")
		Serial.println("CORE2 S3 releasing JTAG for serial");
	#endif

	M5.begin();
	M5.Power.setExtOutput(true);  // enable external bus

	// confusing. this sets font for background display
    M5.Lcd.setTextFont(DEFAULT_FONT);

	// confusing. this sets font for buttons
	M5.Lcd.setFont(WIDGET_FONT);

	M5.Speaker.setAllChannelVolume(70);


    bool ok = SD.begin(SDCARD_CSPIN, SPI, 8000000);
	Serial.printf("SD card is %s READY\n", ok? "NOT":"");
	
	_lfillRect(0, 0, 50, 50, 0x0000FF);
	delay(2000);


	_setup_RTC();
	_setup_ota();		
	_setup_button();

	pinMode(19,INPUT);  // safety . see above.

}

//-------------------------------------------------------------
void _loop_M5(void)
{
	_loop_ota();
}
//-------------------------------------------------------------


void _lsetTextColor(uint32_t FGND, uint32_t BKGND)
{
    M5.Lcd.setTextFont(DEFAULT_FONT);
	//M5.Lcd.setTextColor(FGND);
	M5.Lcd.setTextColor(FGND,BKGND);
	
	//w = M5.Lcd.width();
    //h = M5.Lcd.height();
    //M5.Lcd.setTextColor(TFT_BLACK);
    //M5.Lcd.fillScreen(WHITE);
    //M5.Display.setRotation(1);
    //M5.Display.setTextColor(TFT_BLACK);
    //M5.Display.setTextDatum(top_center);
    //M5.Display.drawString("HI MOM", w / 2, 0, &fonts::FreeMonoBold12pt7b);
    
}


void _reportPower(void)
{
    //M5.Display.clear(TFT_WHITE);
    
    bool isCharging = M5.Power.isCharging();
    int vol_per = M5.Power.getBatteryLevel();
    int vol = M5.Power.getBatteryVoltage();
    int cur = M5.Power.getBatteryCurrent();

	Serial.printf("\n-------------------------\n");
    Serial.printf("Charging: %s\n", isCharging ? "Yes" : "No");
    Serial.printf("Bat_level: %d%%\n", vol_per);
    Serial.printf("Bat_voltage: %d%mV\n", vol);
    Serial.printf("Bat_current: %d%mA\n\n", cur);
}                                              

