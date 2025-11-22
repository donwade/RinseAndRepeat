#include <SD.h>
#include <M5Unified.h>
#include <esp_log.h>

extern void _setup_wavePlayer();
extern void _wavPlayerTask(void *NOTUSED);
extern bool _add_to_playlist(char *filename);

