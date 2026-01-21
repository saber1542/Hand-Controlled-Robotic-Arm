#include<Wire.h>

//Create Sensors
int ring = 0; //Ring finger
int finger = 0; //Index finger
int middle = 0; //Middle finger

int ring_Data = A1;
int finger_Data = A2;
int middle_Data = A3;

//const int MPU_addr = 0x68;
const int MPU1 = 0x68;

//MPU6050
int16_t AcX1, AcY1, AcZ1, Tmp1, GyX1, GyY1, GyZ1;
int minVal = 265;
int maxVal = 402;
double x;
double y;
double z;

//Calibrates sensors
int middle_high = 0;
int middle_low = 0;
int finger_high = 0;
int finger_low = 0;
int ring_high = 0;
int ring_low = 0;

//Stop Caliberating the Flex Sensor when complete
bool bool_caliberate = false;

//How often to send values to the Robotic Arm
int response_time = 100;

void setup() {
  pinMode(3, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);// PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(4800);
  delay(1000);

}
void loop() {

  //Serial.print() would send all values the robotic arm using via bluetooth.
  pinMode(3, HIGH); //Use basic LED as visual indicator if value being sent

  //get values for first mpu having address of 0x68
  GetMpuValue1(MPU1);
  //Serial.prinlnt("  ");
  delay(10);

  //Serial.println("");
  delay(10);

  //Print out a value, based on the change of the XYZ co-ordinates

  //Move Left
  if ( x > 15 && x < 55 && y < 30) {
    Serial.print("L");
    delay(response_time);
  }

  //Move Right
  if ( x < 310 && x > 270) {
    Serial.print("R");
    delay(response_time);
  }

  //Claw Up
  if ( y > 60 && y < 80) {
    Serial.print("G");
    delay(response_time);
  }

  //  //Claw Down
  if ( y < 310 && y > 270) {
    Serial.print("U");
    delay(response_time);
  }

  //  //Move right
  if ( z> 50 && z < 85) {
    Serial.print("C");
    delay(response_time);
  }

  //  //Move left --- Right Hand
  if ( z < 160 && z > 120) {
    Serial.print("c");
    delay(response_time);

  }

  // read the values from Flex Sensors to Arduino
  ring = analogRead(ring_Data);
  finger = analogRead(finger_Data);
  middle = analogRead(middle_Data);


  //Calibrate to find upper and lower limit of the Flex Sensor
  if (bool_caliberate == false ) {
    delay(1000);

    middle_high = (middle * 1.15);
    middle_low = (middle * 0.9);

    finger_high = (finger * 1.03);
    finger_low = (finger * 0.8);

    ring_high = (ring * 1.06);
    ring_low = (ring * 0.8);

    bool_caliberate = true;
  }

  delay(response_time);

  // Ring
  if (ring >= ring_high) {
    Serial.print("P");
    delay(response_time);

  }
  if (ring <= ring_low ) {
    Serial.print("p");
    delay(response_time);
  }


  // middle 1 - middle (Base Rotation)
  if (middle >= middle_high) {
    Serial.print("T");
    delay(response_time);
  }

  if (middle <= middle_low) {
    Serial.print("t");
    delay(response_time);
  }
  //Toggle Safe mode
  if (middle>=middle_high && ring>=ring_high){
    Serial.print("J");
    delay(response_time);
  }

  // finger 1 - Claw Bend/Open
  if (finger >= finger_high) {
    Serial.print("F");
    delay(response_time);
  }

  if (finger <= finger_low) {
    Serial.print("f");
    delay(response_time);
  }
  else {
    delay(5);
  }
}

void GetMpuValue1(const int MPU) {

  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true); // request a total of 14 registers

  AcX1 = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY1 = Wire.read() << 8 |  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ1 = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Tmp1 = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)

  int xAng = map(AcX1, minVal, maxVal, -90, 90);
  int yAng = map(AcY1, minVal, maxVal, -90, 90);
  int zAng = map(AcZ1, minVal, maxVal, -90, 90);

  GyX1 = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY1 = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ1 = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI) + 4; //offset by 4 degrees to get back to zero
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);
}
