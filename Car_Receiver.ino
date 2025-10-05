#define HELTEC_POWER_BUTTON
#include <heltec_unofficial.h>

#include <ESP32Servo.h>

#define DEFAULT_STEER 1530 // Neutral value based on steering wheel at center
#define DEFAULT_GAS   1517 // Neutral value based on gas pedal depressed
const int STEERING_PIN = 2;
const int GAS_PIN      = 3;

int SteeringData;
int GasData;
int BrakeData;

void updateDisplay(int);
void setSteering(int);
void setGas(int);
void setBrake(int);
int smoothing(int, int);

Servo steeringServo;
Servo throttleESC;

void setup()
{
  heltec_setup();
  pinMode(Vext, OUTPUT);
  
  int state = radio.begin(915.0);
  radio.setSpreadingFactor(5);
  radio.setBandwidth(500.0);
  //updateDisplay(state);

  // Initialize outputs that will interface with the car
  steeringServo.attach(STEERING_PIN, 1000, 2000);
  throttleESC.attach(GAS_PIN, 1000, 2000);
}

void loop()
{
  String message;
  int receiveData = 0;
  heltec_loop();

  int state = radio.receive(message);

  if (state == RADIOLIB_ERR_NONE)
  {
    int steeringVal, gasVal, brakeVal;
    
    receiveData = message.toInt();
    Serial.print("Received: ");
    Serial.print(receiveData);

    // Parse received message data
    steeringVal = (receiveData >> 20) + 1000;
    gasVal = ((receiveData >> 10) & 0x3FF) + 1000;
    brakeVal = (receiveData & 0x3FF) + 1000;

    Serial.print("\t");
    Serial.print(steeringVal);
    Serial.print("\t");
    Serial.print(gasVal);
    Serial.print("\t");
    Serial.println(brakeVal);
    
    setSteering(steeringVal);

    // Prioritize braking even if gas and brake pedals are both pressed
    if (abs(brakeVal - DEFAULT_GAS) < 30) // include a deadzone for the brake pedal
      setGas(gasVal);
    else
      setBrake(brakeVal);
  }
  else
  {
    // If signal is lost, failsafe sets gas and steering to neutral
    setSteering(DEFAULT_STEER);
    setGas(DEFAULT_GAS);
    setBrake(DEFAULT_GAS);
  }

}

void setSteering(int steeringVal)
{
  static int prevSteering = 1530;
  steeringVal = smoothing(prevSteering, steeringVal);

  //Reverse the steering if necessary
  //steeringVal = 2000 - steeringVal;
  
  steeringServo.writeMicroseconds(steeringVal);
  prevSteering = steeringVal;
}

void setGas(int gasVal)
{
  // Create a deadzone for the gas pedal
  if (abs(gasVal - DEFAULT_GAS) > 10)
    throttleESC.writeMicroseconds(gasVal);
  else
    throttleESC.writeMicroseconds(DEFAULT_GAS);
}

void setBrake(int brakeVal)
{
  throttleESC.writeMicroseconds(brakeVal);
}

int smoothing(int prevInput, int input)
{
  if (abs(prevInput - input) > 50)
  {
    input = (prevInput + input) / 2;
  }
  return input;
}

void updateDisplay(int state)
{
  /*
  // This section used to display debug info on the car's display
  
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
  */
}
