#include <M5Unified.h>
#include "_onPowerDn.h"
#include "pretty.h"
/*
 * void readPowerButton(void)
 * {
 *      Wire1.beginTransmission(0x34); // AXP192 I2C address
 *      Wire1.write(0x46);			   // Register containing button status
 *      Wire1.endTransmission();
 *      Wire1.requestFrom(0x34, 1);    // Request 1 byte
 *
 *      if (Wire1.available()) {
 *        uint8_t val = Wire1.read();
 *
 *        // Bit 0 = Long press (approx 4+ seconds)
 *        if ((val & 0x01) == 0x01) {
 *              Serial.println("Power button long pressed!");
 *              // Perform safe shutdown here
 *        }
 *
 *        // Bit 1 = Short press / Click
 *        else if ((val & 0x02) == 0x02) {
 *              Serial.println("Power button short clicked!");
 *              // Perform custom action here
 *        }
 *      }
 * }
 */
void setup_onPwrDn(void)
{
    // Read current value of Register 0x36 (PEK/Power Key Parameters)
    Wire1.beginTransmission(0x34);
    Wire1.write(0x36);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);

    if (Wire1.available())
    {
        uint8_t val = Wire1.read();

        // Clear Bit 3 (0 = Disable automatic hardware shutdown on long press)
        val &= ~(1 << 3);

        // Write the modified configuration back to Register 0x36
        Wire1.beginTransmission(0x34);
        Wire1.write(0x36);
        Wire1.write(val);
        Wire1.endTransmission();

        Serial.println("ENSURE YOU CALL loop_onPwrDn in loop()");
    }
}


static pfnv pLongPressCall = NULL;
void setLongPressCB(pfnv userCB)
{
     pLongPressCall = userCB;
}

static pfnv pShortPressCall = NULL;
void setShortPressCB(pfnv userCB)
{
    pShortPressCall = userCB;
}


void loop_onPwrDn()
{
    // Read register 0x46 for button events
    Wire1.beginTransmission(0x34);
    Wire1.write(0x46);
    Wire1.endTransmission();
    Wire1.requestFrom(0x34, 1);

    if (Wire1.available())
    {
        uint8_t status = Wire1.read();

        // Check if Bit 0 is high (Long Press detected)
        if (status & 0x01)
        {
            Serial.println("Intercepted long press! Saving data...");

            // 1. PLACE YOUR DATA SAVING CODE HERE
            // e.g., close files, write to flash, disconnect Wi-Fi
            if (pLongPressCall)
            {
                Serial.printf(FG_BGREEN "calling long press\n" FG_DONE);
                pLongPressCall();
            }
            else
            {
                Serial.printf(FG_BYELLOW "no long press handler registered\n" FG_DONE);
            }

			Serial.println("Powering off now.");

            // 2. Clear the interrupt flags so it doesn't loop
            Wire1.beginTransmission(0x34);
            Wire1.write(0x46);
            Wire1.write(0xFF); // Writing 1s clears the flags
            Wire1.endTransmission();

            delay(500);

            // 3. Manually tell the PMIC to turn off power
            M5.Power.powerOff();

        }

        // short press is lower priority than long press.
        if (status & 02)
        {
        	if (pShortPressCall)
        	{
				Serial.println(FG_BGREEN "calling short press" FG_DONE);
				pShortPressCall();
        	}
        	else
        	{
                Serial.printf(FG_BYELLOW "no short press handler registered\n" FG_DONE);
        	}
	
            // 2. Clear the interrupt flags so it doesn't loop
            Wire1.beginTransmission(0x34);
            Wire1.write(0x46);
            Wire1.write(0x02);     // Writing 1s status flag
            Wire1.endTransmission();

        }
    }
}
