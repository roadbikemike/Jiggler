# ESP32 Bluetooth Mouse Jiggler

ESP32 connects to a PC or mobile phone as a bluetooth mouse and jiggles every few seconds to keep the screen alive.  Code updated to work with the HiLetgo ESP32 LCD WiFi Kit ESP-32 1.14 Inch LCD Display WiFi+Bluetooth CH9102 USB Type-C Internet Development Board for Arduino ESP8266 NodeMCU.  Nicer option with integrated color display.


## Features

- 1.14" Color TFT Display (status, countdown to next jiggle, progress bar, configured interval, configured channel/MAC)
- Start/Pause button (short press on left button - GPIO 0)
- Jiggle Interval configurable by button (short press on right button - GPIO 35)
- Use the jiggler on up to three devices (long press on right button)
- Saves current settings to flash
- Undetectable
- No soldering required - uses built-in buttons
- Simple and reliable

## Known limitations

- If bluetooth connection is lost the ESP is restarted because the BLE library does not perform a proper re-init
- Same for changing the bluetooth MAC address

## Parts

- TTGO T-Display ESP32 (or compatible board with ST7789 1.14" 240x135 TFT display)
  - Example: HiLetgo ESP32 LCD WiFi Kit ESP-32 1.14 Inch LCD
  - Built-in buttons on GPIO 0 and GPIO 35
  - Built-in ST7789V display
  - No additional components needed!

## Setup

- Clone the repo.
- Open the repo with VisualStudio Code with PlatformIO installed.
- Press upload button and wait for upload to finish.
- Power your board on
- Connect to bluetooth device

## Credits

- Cloned from https://github.com/perryflynn/mouse-jiggler
- Original code by https://github.com/tornado67/DroChill


## License

[MIT](https://choosealicense.com/licenses/mit/)
