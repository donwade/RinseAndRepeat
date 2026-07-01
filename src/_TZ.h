#ifndef __MYTZ
#define __MYTZ

#include <Timezone.h>
#include <TimeLib.h> // Or <Time.h>

char *getHHMM(uint32_t utc);
char *getHHMMSS(uint32_t utc);

char *getHHMMapm(uint32_t utc);
char *getHHMMSSapm(uint32_t utc);

char *getDDMMYY(uint32_t utc);
uint32_t getEpochTimeFromGPS();

char *autoFILENAME(char *prefix, char *ext);


extern uint16_t yearNow;
extern uint8_t monthNow;
extern uint8_t dayNow;
extern uint8_t hourNow;
extern uint8_t minuteNow;
extern uint8_t secondNow;
extern uint32_t epochTimeNow;
#endif
