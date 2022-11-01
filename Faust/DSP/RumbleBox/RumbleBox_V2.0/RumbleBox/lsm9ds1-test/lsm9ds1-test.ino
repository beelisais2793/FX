#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LSM9DS1.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();

//Mins and maxs
float xmax = 0;
float xmin = 0;
float ymax = 0;
float ymin = 0;
float zmax = 0;
float zmin = 0;



void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  delay(0.1);
  //initialise min/maxs
  lsm.read();
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp); 
  xmax=m.magnetic.x;
  xmin=m.magnetic.x;
  ymax=m.magnetic.y;
  ymin=m.magnetic.y;
  zmin=m.magnetic.z;
  zmax=m.magnetic.z; 
}


void setup() 
{
  Serial.begin(115200);  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
  }
  Serial.println("Found LSM9DS1 9DOF");

  setupSensor();
}

void loop() 
{
  lsm.read(); 
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp); 

  if(m.magnetic.x>xmax){xmax=m.magnetic.x;};
  if(m.magnetic.x<xmin){xmin=m.magnetic.x;};
  if(m.magnetic.y>ymax){ymax=m.magnetic.y;};
  if(m.magnetic.y<ymin){ymin=m.magnetic.y;};
  if(m.magnetic.z>zmax){zmax=m.magnetic.z;};
  if(m.magnetic.z<zmin){zmin=m.magnetic.z;};

  Serial.print(xmin);Serial.print("\t");
  Serial.print(xmax);Serial.print("\t");
  Serial.print(ymin);Serial.print("\t");
  Serial.print(ymax);Serial.print("\t");
  Serial.print(zmin);Serial.print("\t");
  Serial.print(zmax);
  

  Serial.println();
  delay(20);
}
