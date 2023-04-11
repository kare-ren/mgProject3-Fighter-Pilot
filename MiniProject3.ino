//mgProject 3 - Fighter Pilot
//Program takes in user input from the IMU and capacitive sensor
//Moves the fighter pilot plane based on IMU input from joystick 
//Triggers light and sound feedback through capacitive touch
//Karan Patel and Karen Hughes

//Accelerometer readings from Adafruit ICM20948
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>
#include <CapacitiveSensor.h>

CapacitiveSensor sensor = CapacitiveSensor(A0, 9);
const int buzzerPin = 4;
const int ledPin = 5;
Servo servo_x;
Servo servo_y;
Adafruit_ICM20948 icm;

//For SPI mode, we need a CS pin
#define ICM_CS 10

//For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11

void setup(void) {
  servo_x.attach(2);
  servo_y.attach(3);

  Serial.begin(115200);
  while (!Serial)
    delay(10);

  //Initialize
  if (!icm.begin_I2C()) {
    while (1) {
      delay(10);
    }
  }

  uint16_t accel_divisor = icm.getAccelRateDivisor();
  float accel_rate = 1125 / (1.0 + accel_divisor);

  uint8_t gyro_divisor = icm.getGyroRateDivisor();
  float gyro_rate = 1100 / (1.0 + gyro_divisor);
}

//Delay between measurements for testing
uint16_t measurement_delay_us = 65535;
int x_val;
int y_val;

double my_map(double val, double fromLow, double fromHigh, double toLow, double toHigh){
  return (val - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;
  icm.getEvent(&accel, &gyro, &temp, &mag);

  //Triggers light and sound by capacitive touch input
  long cap_val = sensor.capacitiveSensor(30);
  if(cap_val > 4000){
    tone(buzzerPin, 350, 20);
    digitalWrite(ledPin, HIGH);
    delay(10);
    digitalWrite(ledPin, LOW);
  }
  else
    digitalWrite(ledPin, LOW);

  //Averages IMU input
  x_val = 0;
  y_val = 0;
  for(int i = 0; i < 10; i++){
    x_val += accel.acceleration.x;
    y_val += accel.acceleration.y;
  }
  x_val /= 10;
  y_val /= 10;

  //Prints x and y position for pygame
  Serial.print(y_val*-1);
  Serial.print(" ");
  Serial.println(x_val);

  //Servo motors movement
  x_val = my_map(x_val, -9, 9, 45, 135);
  y_val = my_map(y_val*-1, -9, 9, 45, 135);
  servo_x.write(y_val);
  servo_y.write(x_val);
}
