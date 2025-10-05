#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>

const int STEERING_PIN = 5;
const int GAS_PIN      = 6;
const int BRAKE_PIN    = 7;

int MaxThrottle = 1719; // set a speed limit for the car--can be turned up or down

void updateDisplay(int);
int processSteering(int);
int processGas(int);
int processBrake(int);

void setup()
{
  Serial.begin(115200);
  heltec_setup();
  pinMode(Vext, OUTPUT);

  pinMode(STEERING_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(BRAKE_PIN, INPUT);
  
  int state = radio.begin(915.0);
  radio.setSpreadingFactor(5);
  radio.setBandwidth(500.0);
  //updateDisplay(state);
}

void loop()
{
  String message;
  int encodedMessage, state;
  
  int steeringValue = analogRead(STEERING_PIN);
  int gasValue = analogRead(GAS_PIN);
  int brakeValue = analogRead(BRAKE_PIN);

  // Print raw input data for debugging
  Serial.print(steeringValue);
  Serial.print("\t");
  Serial.print(gasValue);
  Serial.print("\t");
  Serial.print(brakeValue);
  Serial.print("\t");
  
  heltec_loop();

  steeringValue = processSteering(steeringValue);
  gasValue = processGas(gasValue);
  brakeValue = processBrake(brakeValue);

  // Prepare the data payload
  encodedMessage = (steeringValue - 1000) << 20 | (gasValue - 1000) << 10 | (brakeValue - 1000);
  message = String(encodedMessage);
  state = radio.transmit(message);
  
  Serial.println(message);
}

int processSteering(int steeringValue)
{
  if (steeringValue < 0)
    steeringValue = 0;
  else if (steeringValue > 4095)
    steeringValue = 4095;

  // Create steering maps around the wheel's center point
  if (steeringValue < 1928)
    return map(steeringValue, 0, 1928, 2000, 1530);
  else
    return map(steeringValue, 1928, 4095, 1530, 1080);
}

int processGas(int gasValue)
{
  if (gasValue < 430)
    gasValue = 430;
  else if (gasValue > 3500)
    gasValue = 3500;
  return map(gasValue, 430, 3500, 1517, MaxThrottle);
}
int processBrake(int brakeValue)
{
  if (brakeValue > 3450)
    brakeValue = 3450;
  else if (brakeValue < 200)
    brakeValue = 200;
  return map(brakeValue, 3450, 200, 1517, 1238);
}

void updateDisplay(int state)
{
  // This section used to display debug info on the wheel's display

  Serial.println("Serial works");
  // Display
  display.println("Display works");
  // Radio
  display.print("Radio ");
  if (state == RADIOLIB_ERR_NONE) {
    display.println("works");
  } else {
    display.printf("fail, code: %i\n", state);
  }
  // Battery
  float vbat = heltec_vbat();
  display.printf("Vbat: %.2fV (%d%%)\n", vbat, heltec_battery_percent(vbat));
}
