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

#if 0
const char* ssid     = "M5wifi";
const char* password = "21213123";
#else
static const char* ssid     = MY_SSID;
static const char* password = MY_SSID_PASSWORD;
#endif

//-------------------------------------------------------------

void _setup_ota(void) 
{
    // done elsewhere  M5.begin();

    WiFi.begin(ssid, password);  // Connect wifi and return connection status.

    M5.Lcd.print("Waiting 4 Wifi");
    while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
        M5.Lcd.print(".");
    }
    
    M5.Lcd.println();
    M5.Lcd.print("Online:");
    M5.Lcd.println(WiFi.SSID());  // Output Network name.  输出网络名称
    M5.Lcd.print("IP: ");
    M5.Lcd.println(WiFi.localIP());  // Output IP Address.  输出IP地址
	M5.Lcd.print("Hostname:");
	M5.Lcd.println(REMOTE_HOSTNAME);

	uint8_t baseMac[6];
	esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
	if (ret == ESP_OK) {
	M5.Lcd.printf("MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
				 baseMac[0], baseMac[1], baseMac[2],
				 baseMac[3], baseMac[4], baseMac[5]);
	}
	 
    ArduinoOTA.setHostname(REMOTE_HOSTNAME);  // Set the network port name.  设置网络端口名称
    //ArduinoOTA.setPassword("666666");
    
    ArduinoOTA.begin();            // Initialize the OTA.  初始化OTA
    M5.Lcd.println("OTA ready!");  // M5.Lcd port output format string.
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

