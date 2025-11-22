//#define setCursor(...) M5.Lcd.setCursor(__VA_ARGS__)
//#define print(...) M5.Lcd.print(__VA_ARGS__)
extern void _lsetCursor(uint16_t X, uint16_t Y, uint8_t font);
extern void _lsetCursor(uint16_t X, uint16_t Y);
extern void _lprint(const __FlashStringHelper *x);
extern void _lprint(uint8_t x);
extern void _lprint(char *x);

extern void _lprint(double x, int y);

extern void _lprintln(void);
extern void _lprintln(char *x);
extern void _lclear(void);
extern void _setup_M5(void);
extern void _lsetTextColor(uint32_t FGND, uint32_t BKGND);

#define WIDGET_FONT  &fonts::FreeMonoBoldOblique12pt7b
#define DEFAULT_FONT &fonts::FreeMono12pt7b





