#include <M5Unified.h>

//typedef void *(*pfnv)(void);
typedef void (*pfnv)(void);

void setup_onPwrDn(void);
void loop_onPwrDn();
void setLongPressCB(pfnv userCB);
void setShortPressCB(pfnv userCB);

