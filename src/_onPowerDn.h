#include <M5Unified.h>

typedef void *(*pfnv)(void);

void setup_onPwrDn(void);
pfnv setLongResetPressCB(pfnv userCB);
void loop_onPwrDn();
pfnv setLongResetPressCB(pfnv userCB);
pfnv setShortRestPressCB(pfnv userCB);

