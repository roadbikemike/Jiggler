# ESP32 Bluetooth Mouse Jiggler

ESP32 connects to a PC or mobile phone as a bluetooth mouse and jiggles every few seconds to keep the screen alive.  Code updated to work with the HiLetgo ESP32 LCD WiFi Kit ESP-32 1.14 Inch LCD Display WiFi+Bluetooth CH9102 USB Type-C Internet Development Board for Arduino ESP8266 NodeMCU.  Nicer option with integrated color display.


## Features

- OLED Display (status, countdown to next jiggle, configured interval, configured channel/MAC)
- Start/Pause button (short press on upper button)
- Jiggle Interval configurable by button (short press on lower button)
- Use the jiggler on up to three devices (long press on lower button)
- Saves current settings to flash
- Undetectable
- No setup required
- Simple and reliable

## Known limitations

- If bluetooth connection is lost the ESP is restarted because the BLE library does not perform a proper re-init
- Same for changing the bluetooth MAC address

## Parts

- HiLetgo ESP32 LCD WiFi Kit
- 2x push button

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
