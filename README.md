# KeyPride

An Arduino Nano sketch to create colorful animations on the backlight of a keyboard.

I modded my keyboard, *specifically* an old Logitech G11, the code is specific to the wiring that i designed.
Also the rotary encoder class is specific to the one used for volume control.

## Features

- RGB LED lighting with dynamic color effects
- Arduino Nano compatible
- Easy to customize and extend
- AI-generated bullet points

## Hardware Requirements

- Arduino Nano microcontroller
- WS2812B (NeoPixel) or similar addressable RGB LEDs
- USB cable for programming and power
- A Logitech G11 gaming keyboard

## Installation

1. Connect your Arduino Nano to your computer
2. Open the sketch in the Arduino IDE
3. Install any required libraries (e.g., Adafruit NeoPixel if using WS2812B LEDs)
4. Upload the sketch to your Arduino Nano
5. Open the keyboard
6. Remove the old LED PCBs and try to fit the LED strip in their place
7. Solder all necessary wires (diagram to come)
8. Enjoy RGB backlight

## Usage

The keyboard will automatically start with its default LED pattern, no storage of the last used effect is implemented.
- Press Scroll Lock to activate control mode
- The volume wheel cycles the various animations
- The original lit/dim buttons control the brightness as before

## Customization

Edit the sketch to:
- Change LED colors and patterns
- Adjust animation speed
- Add new lighting effects

## Improvements (not implemented at the moment)

- Store the last selected effect at power off
- Route the volume encoder signals through the arduino board (cut original PCB traces) and block volume change while Scroll Lock is active

## License

See LICENSE file for details.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests.
