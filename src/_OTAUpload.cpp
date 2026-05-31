/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*     Equipped with M5Core2 sample source code
* Visit for more information: https://docs.m5stack.com/en/core/core2
*
* Describe: OTA Upload.
* Date: 2021/7/30
*******************************************************************************
*/

#include <M5Unified.h>
#include <esp_wifi.h>
#include <WiFi.h>

#include <ArduinoOTA.h>

#include "_OTAUpload.h"

#ifndef REMOTE_HOSTNAME
    #define REMOTE_HOSTNAME "NEED-DNS"
#endif

#if !defined(MY_SSID)
 #error provide ssid and password
#else
static const char* ssid     = MY_SSID;
static const char* password = MY_SSID_PASSWORD;
#endif

//-------------------------------------------------------------

void _setup_ota(void) 
{
    // done elsewhere  M5.begin();
    int i;

    M5.Lcd.printf("SSID %s\n", ssid);

   
    WiFi.begin(); // you have to start Wifi to turn it OFF (huh?)
    WiFi.setAutoReconnect(false); 
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    delay(1000);

/*
/home/dwade/.arduino15/packages/esp32/hardware/esp32/3.3.8/libraries/WiFi/src/WiFiType.h:49:
 43 typedef enum {
 44   WL_NO_SHIELD = 255,  // for compatibility with WiFi Shield library
 45   WL_STOPPED = 254,
 46   WL_IDLE_STATUS = 0,
 47   WL_NO_SSID_AVAIL = 1,
 48   WL_SCAN_COMPLETED = 2,
 49   WL_CONNECTED = 3,
 50   WL_CONNECT_FAILED = 4,
 51   WL_CONNECTION_LOST = 5,
 52   WL_DISCONNECTED = 6
 53 } wl_status_t;

*/
    #define STOP 254
    uint32_t snore = millis();

    while(millis() < snore + 4000)
    {
        uint32_t foo = WiFi.status();
        M5_LOGI("wifi wait for disc or stop ret = %d ", foo);
        //if (foo == WL_DISCONNECTED || foo == WL_STOPPED) break;
        if (foo == WL_DISCONNECTED || foo == STOP) break;
        delay(500);
    }

    M5_LOGI("wifi stopped, bringing back up ...");

    WiFi.begin(ssid, password);  // Connect wifi and return connection status.
    for (int i = 0; i < 8; i++)
    {
        uint32_t foo;
        foo = WiFi.status();
        M5_LOGW("wait for wifi %d of 8 tries err = %d\r", i, foo);
        if (foo == WL_CONNECTED) break;
        delay(1000);
    }

    String temp = WiFi.localIP().toString();
    M5_LOGW("IP: %s ", temp.c_str());

	uint8_t baseMac[6];
	uint32_t bigMacLo;
	
	esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);

	if (ret == ESP_OK) 
    {
	    //M5.Lcd.printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	    M5_LOGW("MAC %02x:%02x:%02x:%02x:%02x:%02x\n", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	}
	
	bigMacLo=baseMac[5]       | baseMac[4] <<  8 | 
			 baseMac[3] << 16 | baseMac[2] << 24 ;
	
	char hName[40] = "UNKNOWN"; //REMOTE_HOSTNAME;

	// overide hostname based on MAC
	if (bigMacLo == 0x84A7024C ) strcpy (hName, "YELLOW");
	if (bigMacLo == 0xA0D4CB8C ) strcpy (hName, "BLACK");
    if (bigMacLo == 0x0fdfb2e0 ) strcpy (hName, "GOLD");
    if (bigMacLo == 0x0fdfbae0 ) strcpy (hName, "SILVER");

    ArduinoOTA.setHostname(hName);
    //ArduinoOTA.setPassword("666666");

	M5.Lcd.print("Hostname:");
	M5.Lcd.println(hName);
    M5_LOGW("Hostname: %s", hName);

    ArduinoOTA.begin();
    //M5.Lcd.println("OTA ready!");
}

void _loop_ota(void) 
{
    ArduinoOTA.handle();  // Continuously check for update requests.
}
//-------------------------------------------------------------
void _end_ota(void) {
	M5.Lcd.println("OTA End!");
#if 0
//    M5.update();
//    if (M5.BtnA.isPressed()) {  // if BtnA is Pressed.
//        ArduinoOTA.end();       // Ends the ArduinoOTA service.  结束OTA服务
//    }
#else
      ArduinoOTA.end();       // Ends the ArduinoOTA service.  结束OTA服务
#endif
      delay(200);
}

