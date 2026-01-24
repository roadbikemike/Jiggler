// User setup for ST7789 240x135 TFT (HiLetgo ESP32 LCD)
#define ST7789_DRIVER
#define CGRAM_OFFSET        // CRITICAL: Fix pixel addressing for TTGO T-Display
#define TFT_WIDTH 240
#define TFT_HEIGHT 135
#define TFT_RGB_ORDER TFT_RGB  // RGB color order for TTGO T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4
#define TOUCH_CS -1
#define SPI_FREQUENCY 27000000
#define SPI_READ_FREQUENCY 20000000
#define SPI_TOUCH_FREQUENCY 2500000
#define TFT_INVERSION_ON    // CRITICAL: Fix color/pixel corruption for TTGO T-Display