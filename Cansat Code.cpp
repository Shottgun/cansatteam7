#include <Wire.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <vector>
#include <string.h>
#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial mySerial(8, 9); // 9 = Soft RX pin (not used), 8 = Soft TX pin
Servo dropServo;

Adafruit_LIS3MDL lis3mdl;
Adafruit_LSM6DSOX sox;
Adafruit_BMP3XX bmp;
#define SEALEVELPRESSURE_HPA (1019)

int stage = 0;
String fstate = "awaiting launch";
int i;
const int ID = 1007;
const int groundAlt = 0;
int packs = 0;
double alt_offset;
double alt;
double preAlt = 0;
bool satReleased = false;
int e = 0;

//Set up XBee so warning messages will show up to ground station||Remove while loops after testing is done so it does not pause :D
void BMP_setup() {
  Serial.println("Adafruit BMP388 test");

  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
  }
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}
void lis3mdl_setup() {
  Serial.println("Adafruit LIS3MDL test");

  // Try to initialize!
  if (! lis3mdl.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Failed to find LIS3MDL chip");
  }
  Serial.println("LIS3MDL Found!");

  lis3mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);
  Serial.print("Performance mode set to: ");
  switch (lis3mdl.getPerformanceMode()) {
    case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
    case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
    case LIS3MDL_HIGHMODE: Serial.println("High"); break;
    case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
  }
  
  lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
  Serial.print("Operation mode set to: ");
  // Single shot mode will complete conversion and go into power down
  switch (lis3mdl.getOperationMode()) {
    case LIS3MDL_CONTINUOUSMODE: Serial.println("Continuous"); break;
    case LIS3MDL_SINGLEMODE: Serial.println("Single mode"); break;
    case LIS3MDL_POWERDOWNMODE: Serial.println("Power-down"); break;
  }

  lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
  // You can check the datarate by looking at the frequency of the DRDY pin
  Serial.print("Data rate set to: ");
  switch (lis3mdl.getDataRate()) {
    case LIS3MDL_DATARATE_0_625_HZ: Serial.println("0.625 Hz"); break;
    case LIS3MDL_DATARATE_1_25_HZ: Serial.println("1.25 Hz"); break;
    case LIS3MDL_DATARATE_2_5_HZ: Serial.println("2.5 Hz"); break;
    case LIS3MDL_DATARATE_5_HZ: Serial.println("5 Hz"); break;
    case LIS3MDL_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3MDL_DATARATE_20_HZ: Serial.println("20 Hz"); break;
    case LIS3MDL_DATARATE_40_HZ: Serial.println("40 Hz"); break;
    case LIS3MDL_DATARATE_80_HZ: Serial.println("80 Hz"); break;
    case LIS3MDL_DATARATE_155_HZ: Serial.println("155 Hz"); break;
    case LIS3MDL_DATARATE_300_HZ: Serial.println("300 Hz"); break;
    case LIS3MDL_DATARATE_560_HZ: Serial.println("560 Hz"); break;
    case LIS3MDL_DATARATE_1000_HZ: Serial.println("1000 Hz"); break;
  }
  
  lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
  Serial.print("Range set to: ");
  switch (lis3mdl.getRange()) {
    case LIS3MDL_RANGE_4_GAUSS: Serial.println("+-4 gauss"); break;
    case LIS3MDL_RANGE_8_GAUSS: Serial.println("+-8 gauss"); break;
    case LIS3MDL_RANGE_12_GAUSS: Serial.println("+-12 gauss"); break;
    case LIS3MDL_RANGE_16_GAUSS: Serial.println("+-16 gauss"); break;
  }

  lis3mdl.setIntThreshold(500);
  lis3mdl.configInterrupt(false, false, true, // enable z axis
                          true, // polarity
                          false, // don't latch
                          true); // enabled!
}
void lsm6dsox_setup() {
  Serial.println("Adafruit LSM6DSOX test!");

  if (!sox.begin_I2C()) {
    Serial.println("Failed to find LSM6DSOX chip");
  }

  Serial.println("LSM6DSOX Found!");

  // sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (sox.getAccelRange()) {
  case LSM6DS_ACCEL_RANGE_2_G: Serial.println("+-2G"); break;
  case LSM6DS_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DS_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DS_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  // sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS );
  Serial.print("Gyro range set to: ");
  switch (sox.getGyroRange()) {
  case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    Serial.println("Unexpected Range!");
    break; // unsupported range for the DSOX
  }

  // sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Accelerometer data rate set to: ");
  switch (sox.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  // sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Gyro data rate set to: ");
  switch (sox.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }
}

void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
//  OpenLog.begin(9600); //Open software serial port at 9600bps
  delay(20000); //change to 20 secs
  Serial.println("Pico started sending bytes via XBee");

  dropServo.attach(6,0, 3000);
  dropServo.write(120);
  pinMode(26, OUTPUT); //Buzzer
  pinMode(22, OUTPUT); //LED
  
  BMP_setup();
  
  lis3mdl_setup();
  
}

void release_cansat() {
  dropServo.write(60);
  delay(300);
}
void configure_bmp() {
  double num1;
  double num2;
  double num3;
  double num4;
  double num5;
  bmp.readAltitude(SEALEVELPRESSURE_HPA);
  delay(3000);
  for(int i=0;i<6;i++) {
    switch(i){
      case 1: num1 = bmp.readAltitude(SEALEVELPRESSURE_HPA); break;
      case 2: num2 = bmp.readAltitude(SEALEVELPRESSURE_HPA); break;
      case 3: num3 = bmp.readAltitude(SEALEVELPRESSURE_HPA); break;
      case 4: num4 = bmp.readAltitude(SEALEVELPRESSURE_HPA); break;
      case 5: num5 = bmp.readAltitude(SEALEVELPRESSURE_HPA); break;
      default: true; break;
    }
  }
  alt_offset = (num1 + num2 + num3 + num4 + num5)/5;
  Serial.print(alt_offset);
}
void get_data() {
  //bmp388 part
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading D:");
  }

  //--*For Transmitt*--||
  bmp.performReading();
  lis3mdl.read();      // get X Y and Z data at once

  sensors_event_t event; 
  lis3mdl.getEvent(&event);

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sox.getEvent(&accel, &gyro, &temp);

  unsigned long tme = millis();
  int hr = tme/3600;
  int mins = (tme-hr*3600)/60;
  int sec = tme-hr*3600-mins*60;
                                           
  String mtime = (String(hr) + ":" + String(mins) + ":" + String(sec));
  double v = analogRead(22);
  double roll = gyro.gyro.x;
  double pitch = gyro.gyro.y; 
  double yaw = gyro.gyro.z;
  double tempe = bmp.temperature;
  double pres = bmp.pressure / 100;
  alt = bmp.readAltitude(SEALEVELPRESSURE_HPA) - alt_offset;
  double lati = 0;
  double longi = 0;
  double dirX = lis3mdl.x;
  double dirY = lis3mdl.y;
  double dirZ = lis3mdl.z;
  double eveX = event.magnetic.x;
  double eveY = event.magnetic.y;
  double eveZ = event.magnetic.z;
  packs += 1;
  String c = ",";
  Serial.println(String(ID) + c + String(mtime) + c + String(packs) + c + String(fstate) + 
  c + String(stage) + c + String(alt) + c + String(tempe) + c + String(pres) + c + String(v) + 
  c + String(lati) + c + String(longi) + c + String(roll) + c + String(pitch) + c + String(yaw));
  //XBee
  mySerial.println(String(ID) + c + String(mtime) + c + String(packs) + c + String(fstate) + 
  c + String(stage) + c + String(alt) + c + String(tempe) + c + String(pres) + c + String(v) + 
  c + String(lati) + c + String(longi) + c + String(roll) + c + String(pitch) + c + String(yaw) + "|");
}

void flight_stage_1() {
  configure_bmp();
  Serial.println("Stage 1");
  if ( alt < (groundAlt + 20) ) {
    Serial.println("yes");
    digitalWrite(22, HIGH);
    delay(500);
    digitalWrite(22, LOW);
    get_data();
    delay(1000);
  }
  else {
    Serial.print("no");
    stage++;
    fstate = "Flying";
  }
}
void flight_stage_2() {
  Serial.println("Stage 2");
  if ( alt < 460 ) {
    Serial.print("what the heck");
    digitalWrite(22, HIGH);
    delay(500);
    digitalWrite(22, LOW);
    get_data();
    delay(1000);
  }
  else {
    stage++;
    fstate = "Descending";
  }
}
void flight_stage_3() {
  Serial.println("Stage 3");
  Serial.print("fly");
  if( satReleased == false ) {
    release_cansat();
    satReleased = true;
  }
  preAlt = alt;
  delay(2000);
  get_data();
  digitalWrite(22, HIGH);
  delay(500);
  digitalWrite(22, LOW);
  if ( preAlt <= alt+3 && preAlt >= alt-3 ) {
    e++;
    if( e == 2 ) {
      stage++;
      fstate = "Landed!";
    }
  }
}
void flight_stage_4() {
  Serial.println("Stage 4:");
  get_data();
  digitalWrite(22, HIGH);
  delay(500);
  digitalWrite(22, LOW);
  digitalWrite(26, HIGH);
  delay(800);
  digitalWrite(26, LOW);
  Serial.print(stage);
}


void loop() {
  switch(stage) {
    case 0: stage++; break;
    case 1: flight_stage_1(); break;
    case 2: Serial.print("to Stage 2"); flight_stage_2(); break;
    case 3: Serial.print("to Stage 3"); flight_stage_3(); break;
    case 4: Serial.print("to Stage 4"); dropServo.write(120); flight_stage_4(); break;
    default: "Case Error"; break;
  }
}
