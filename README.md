# RC Transmitter / Receiver (ESP32, Arduino IDE)

Two sketches for a simple RC link:
- **Transmitter** reads steering wheel/pedals and sends control data.
- **Receiver** drives servos/ESCs (PWM) and/or a PPM stream, with a failsafe.

> Target boards: ESP32 dev boards (Heltec, DOIT, etc.). 

## Features
- Low-latency wireless link
- 2 analog output channels and 3 analog input channels
- Receiver outputs standard **servo/ESC PWM** (50 Hz)
- Failsafe: neutral or motor-off on link loss


## Hardware

### Example pins (use available analog pins)

**Transmitter**
| Function        | HELTEC Pin | Notes              |
|-----------------|------------|--------------------|
| Steering (ADC)  |          5 | 10–100 kΩ pot      |
| Gas Pedal       |          6 | INPUT_PULLUP       |
| Brake Pedal     |          7 | INPUT_PULLUP       |

**Receiver**
| Function               | HELTEC Pin | Notes                       |
|------------------------|------------|-----------------------------|
| ESC / Throttle (PWM)   |          3 | 1000–2000 µs @ 50 Hz        |
| Steering Servo (PWM)   |          2 | 1000–2000 µs @ 50 Hz        |

**Power Requirements**
- Select a battery to match the motor voltage requirements.
- Power servos/ESCs from a suitable BEC/external supply; **share GND** with the ESP32.
- ESP32 may be powered from the 5V supply of the BEC if using the USB or 5V input.
- GPIO should not be connected to 5V since ESP32 logic level is 3.3V.


## Software Setup (Arduino IDE)

1. **Install ESP32 boards**  
   Arduino IDE → Boards Manager → search “esp32” → install **“ESP32 by Espressif Systems”**.

2. **Open and configure sketches**
   - Open `receiver/receiver.ino`, select your ESP32 board.
   - Download and install the heltec_unofficial library.
   - Configure both radio frequencies, the spreading factor, and the bandwidth.

3. **Serial monitor**  
   Both sketches use **115200 baud**.


## Packet Format (example)

Payload is prepared as a 9-digit value. Since data range is 1000-2000, the thousand place is ignored. Steering, gas, and brake values are encoded sequentially as a string.

Receiver checks for incoming data. If none is received (failsafe) outputs go to neutral (or motor-off and servo centered). Control will resume when communication is reestablished.

## Tuning & Calibration
Dead-zone around joystick center to suppress noise (e.g., ±20).
Scaling to shape response curves.
Endpoints clamp to safe servo/ESC range (1000–2000 µs).
Failsafe values set near the top of receiver.ino.

## Safety
Test with wheels off the ground first.
Confirm failsafe cuts throttle on link loss.
Use appropriate power rails for servos/ESCs; never drive motors from ESP32 3.3 V--tapping into 5V output of BEC is recommended.
Keep antennas clear of metal and high-current wiring.

## Troubleshooting
Jitter: add averaging on ADC; ensure servo/BEC power is clean; twist/shorten analog wiring.
Boot twitch: set PWM to neutral before enabling servo/ESC power.

## Future Versions
Add data encryption and checksum.
Return telemetry data from car and display on steering wheel
