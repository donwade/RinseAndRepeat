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
const char* ssid     = "ignite wifi";
const char* password = "DEADBEEF04";
#else
static const char* ssid     = MY_SSID;
static const char* password = MY_SSID_PASSWORD;
#endif

//-------------------------------------------------------------

void _setup_ota(void) 
{
    // done elsewhere  M5.begin();

    M5.Lcd.printf("SSID %s\n", ssid);
    WiFi.begin(ssid, password);  // Connect wifi and return connection status.

    while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        M5.Lcd.print(".");
    }
    
    //M5.Lcd.println();
    //M5.Lcd.print("Online:");
    //M5.Lcd.println(WiFi.SSID());  // Output Network name.  输出网络名称
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());  // Output IP Address.  输出IP地址

	uint8_t baseMac[6];
	uint32_t bigMacLo;
	
	esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);

    M5.Lcd.printf("BUILT: %s %s\n", __DATE__,__TIME__);

	if (ret == ESP_OK) {
	M5.Lcd.printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				 baseMac[0], baseMac[1], baseMac[2],
				 baseMac[3], baseMac[4], baseMac[5]);
	}
	
	bigMacLo=baseMac[5]       | baseMac[4] <<  8 | 
			 baseMac[3] << 16 | baseMac[2] << 24 ;
	
	char hName[40] = "UNKNOWN"; //REMOTE_HOSTNAME;

	// overide hostname based on MAC
	if (bigMacLo == 0x84A7024C ) strcpy (hName, "YELLOW");
	if (bigMacLo == 0xA0D4CB8C ) strcpy (hName, "BLACK");

    ArduinoOTA.setHostname(hName);
    //ArduinoOTA.setPassword("666666");

	M5.Lcd.print("Hostname:");
	M5.Lcd.println(hName);
    
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

