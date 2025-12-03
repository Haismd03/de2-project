# FM radio receiver with digital tuning

## License

## Team members
- Tadeáš Fojtách (responsible for OLED display configuration)
- David Haisman (responsible for control peripherals)
- Karel Matoušek (responsible for radio module configuration)

## Abstract
This projet deals with controling the radio module SI4703 using Arduino UNO microcontroller and C programming language. It's goal is to sweep through the radio stations typically receivable in Brno, determine those which radio is really able to receive, and provide them to the user. Two rotary encoders are used, one for radio tuning and one for the volume control. To inform the user about the currently tuned station and frequency, these data is displayed on the small OLED display.

## Scheme with complete wiring of the device

## Usage manual

## Demonstrative video

## Used hardware

  ### Arduino UNO

  ### SI4703
  SI4703 is a radio module used for receiving FM radio broadcast. I uses 3.3V logic and I2C to communicate with the controller. To activate the receiver, some device, that can be used as antenna (for example headphones), must be plugged in to the connector. This device is also used for reproducing the decoded received radio signal. 

  ### SSD1306

  ### Rotary encoder

## Used third-party codes and libraries

## Documentation

## References
