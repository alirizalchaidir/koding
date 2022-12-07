// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
//
// Tested and works great with the Adafruit Ultimate GPS module
// using MTK33x9 chipset
//    ------> http://www.adafruit.com/products/746
// Pick one up today at the Adafruit electronics shop
// and help support open source hardware & software! -ada

#include <Adafruit_GPS.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
//#include <SoftwareSerial.h>
double SP_A = 0;
int SP_A_int = 0;
int SP_A_cek = 0;
double SP_O = 0;
int SP_O_int = 0;
int SP_O_cek = 0;
int tahap = 0;
int arah;
int klaster;

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

float headingDegrees;//hmc
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);//hmc
// Connect the GPS Power pin to 5V
// Connect the GPS Ground pin to ground
// Connect the GPS TX (transmit) pin to Digital 8
// Connect the GPS RX (receive) pin to Digital 7

// you can change the pin numbers to match your wiring:
//SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&Serial2);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true

void setup()
{
 pinMode(14, OUTPUT); //kiri
 pinMode(27, OUTPUT);  //depan ok
 pinMode(25, OUTPUT); //tengah
 pinMode(33, OUTPUT); //belakang ok 
 pinMode(32, OUTPUT); //kanan
 pinMode(36, INPUT);
 
 pinMode(18, OUTPUT);
 pinMode(19, OUTPUT);
 pinMode(5, OUTPUT);

digitalWrite(14, LOW);
digitalWrite(32, LOW);
digitalWrite(27, LOW);
digitalWrite(33, LOW);
digitalWrite(25, LOW);

//maju = 18 dan 19 smadengan low
//belok kiri = 18 high dan 19 low
//belok kanan = 18 low dan 19 high
digitalWrite(18, HIGH);
digitalWrite(19, HIGH);
digitalWrite(5, LOW);

 
  lcd.init();
  lcd.backlight();
    // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);
  delay(5000);
  Serial.println("Adafruit GPS library basic test!");

  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);
  // Ask for firmware version
  Serial2.println(PMTK_Q_RELEASE);

  //hmc
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
}

uint32_t timer = millis();
void loop()                     // run over and over again
{
  
  baca_hmc();
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if ((c) && (GPSECHO))
    Serial.write(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer

    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      lcd.setCursor(0, 0);
      lcd.print(GPS.latitude, 3);
      lcd.setCursor(0, 1);
      lcd.print(GPS.longitude, 3);

      SP_A = (double)GPS.latitude, 3;
      SP_A = SP_A * 1000;
      SP_A_cek = round(SP_A);

      
      SP_O = (double)GPS.longitude, 3;
      SP_O = SP_O * 1000;
      SP_O_cek = round(SP_O);
      
      lcd.setCursor(10,0);
      lcd.print(SP_A_int);

//------------------cek kompas
  if ((arah <= 358) && (arah >=298))
 {
  klaster = 1;
  lcd.setCursor(8,0);
  lcd.print(klaster);
 }

 if ((arah < 298) && (arah >= 233))
 {
  klaster = 2;
  lcd.setCursor(8,0);
  lcd.print(klaster);
 }

 if ((arah < 233) && (arah >= 100))
 {
  klaster = 3;
  lcd.setCursor(8,0);
  lcd.print(klaster);
 }

 if ((arah < 100) && (arah >= 0))
 {
  klaster = 4;
  lcd.setCursor(8,0);
  lcd.print(klaster);
 }
//---------------------------

      
      if (digitalRead(36)==LOW)
      {
        //SP_A = (double)GPS.latitude, 3;
        //SP_A = SP_A * 1000;
        SP_A_int = round(SP_A);
        lcd.setCursor(10,0);
        lcd.print(SP_A_int);

        //SP_O = (double)GPS.longitude, 3;
        //SP_O = SP_O * 1000;
        SP_O_int = round(SP_O);
        
        tahap = 1;
      }

      if (tahap == 1)
      {
        if ((arah >= 353) && (arah <= 360)) //356-360
          {
             if (SP_A_cek < (SP_A_int + 2))
              {
               digitalWrite(14, HIGH);   //maju
               digitalWrite(32, HIGH);
               digitalWrite(27, LOW);
               digitalWrite(33, HIGH);
               digitalWrite(25, HIGH);

               digitalWrite(18, LOW);
               digitalWrite(19, LOW);
               digitalWrite(5, LOW);
              }
             else
             {
             //tanda untuk tugas berikutnya
               digitalWrite(14, HIGH);
               digitalWrite(32, HIGH);
               digitalWrite(27, HIGH);
               digitalWrite(33, HIGH);
               digitalWrite(25, LOW); 
               tahap = 2;
               SP_A_int = SP_A_cek;
               SP_O_int = SP_O_cek;
               
               //belok kanan
                baca_hmc();
                while (arah > 300)
                {
                  digitalWrite(18, HIGH);
                  digitalWrite(19, LOW);
                  digitalWrite(5, HIGH);
                  baca_hmc();
                  //delay(2000);
                } 
             }
           }
          else
          {
              //robot berputar ke arah kiri mencari set point
              if ((klaster == 1) || (klaster == 2))
              {
                digitalWrite(14, LOW);
                digitalWrite(32, HIGH);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);

                digitalWrite(18, HIGH);
                digitalWrite(19, LOW);
                digitalWrite(5, LOW);
              }

              else
              {
                digitalWrite(14, HIGH);
                digitalWrite(32, LOW);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);

                digitalWrite(18, LOW);
                digitalWrite(19, HIGH);
                digitalWrite(5, LOW);
              }
          }        
        //cari_arah_selatan();
      }

      if (tahap == 2)
      {
        if ((arah >= 293) && (arah <= 300)) //296-300
          {
            if (SP_O_cek > (SP_O_int - 2))
              {
                digitalWrite(14, HIGH);   //maju
                digitalWrite(32, HIGH);
                digitalWrite(27, LOW);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);

                digitalWrite(18, LOW);
                digitalWrite(19, LOW);
                digitalWrite(5, LOW);
              }
            else
              {
                //tanda untuk tugas berikutnya 
                digitalWrite(14, HIGH);
                digitalWrite(32, HIGH);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, LOW);
                tahap = 3;
                SP_A_int = SP_A_cek;
                SP_O_int = SP_O_cek;

                //belok kanan
                 baca_hmc();
                while (arah > 235)
                {
                  digitalWrite(18, HIGH);
                  digitalWrite(19, LOW);
                  digitalWrite(5, HIGH);
                  baca_hmc();
                  //delay(2000);
                }               
              }
           }
        else
          {
            //robot berputar ke arah kiri mencari set point
              if ((klaster == 2) || (klaster == 3))
              {
                digitalWrite(14, LOW);
                digitalWrite(32, HIGH);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);
                
                digitalWrite(18, HIGH);
                digitalWrite(19, LOW);
                digitalWrite(5, LOW);
              }

              else
              {
                digitalWrite(14, HIGH);
                digitalWrite(32, LOW);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);

                digitalWrite(18, LOW);
                digitalWrite(19, HIGH);
                digitalWrite(5, LOW);
              }
            } 
        //cari_arah_barat();
       }

      if (tahap == 3)
      {
          if ((arah >= 228) && (arah <= 235)) //231-235
            {
              if (SP_A_cek > (SP_A_int - 4))
                {
                  digitalWrite(14, HIGH);   //maju
                  digitalWrite(32, HIGH);
                  digitalWrite(27, LOW);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, HIGH);

                  digitalWrite(18, LOW);
                  digitalWrite(19, LOW);
                  digitalWrite(5, LOW);
                }
              else
                {
                  //tanda untuk tugas berikutnya 
                  digitalWrite(14, HIGH);
                  digitalWrite(32, HIGH);
                  digitalWrite(27, HIGH);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, LOW);
                  tahap = 4;
                  SP_A_int = SP_A_cek;
                  SP_O_int = SP_O_cek;
                  
                  //belok kanan
                  baca_hmc();
                while (arah > 102)
                {
                  digitalWrite(18, HIGH);
                  digitalWrite(19, LOW);
                  digitalWrite(5, HIGH);
                  baca_hmc();
                  //delay(2000);
                }
                }
            }
          else
            {
              //robot berputar ke arah kiri mencari set point
              if ((klaster == 3) || (klaster == 4))
              {
                digitalWrite(14, LOW);
                digitalWrite(32, HIGH);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);
              
                digitalWrite(18, HIGH);
                digitalWrite(19, LOW);
                digitalWrite(5, LOW);
              }

              else
              {
                digitalWrite(14, HIGH);
                digitalWrite(32, LOW);
                digitalWrite(27, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);
              
                digitalWrite(18, LOW);
                digitalWrite(19, HIGH);
                digitalWrite(5, LOW);
              }
            }
        //cari_arah_utara();
      }
      
      if (tahap == 4)
      {
          if ((arah >= 95) && (arah <= 102))  //98-102
            {
              if (SP_O_cek < (SP_O_int + 4))
                {
                  digitalWrite(14, HIGH);   //maju
                  digitalWrite(32, HIGH);
                  digitalWrite(27, LOW);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, HIGH);
                 
                  digitalWrite(18, LOW);
                  digitalWrite(19, LOW);
                  digitalWrite(5, LOW);
                }
              else
                {
                  //tanda untuk tugas berikutnya 
                  digitalWrite(14, HIGH);
                  digitalWrite(32, HIGH);
                  digitalWrite(27, HIGH);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, LOW);
                  tahap = 0;
                  SP_A_int = SP_A_cek;
                  SP_O_int = SP_O_cek;

                  //berhenti
                  digitalWrite(18, HIGH);
                  digitalWrite(19, HIGH);
                }
             }
           else
            {
              //robot berputar ke arah kiri mencari set point
              if ((klaster == 4) || (klaster == 1))
                {
                  digitalWrite(14, LOW);
                  digitalWrite(32, HIGH);
                  digitalWrite(27, HIGH);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, HIGH);

                  digitalWrite(18, HIGH);
                  digitalWrite(19, LOW);
                  digitalWrite(5, LOW);
                }

               else
                {
                  digitalWrite(14, HIGH);
                  digitalWrite(32, LOW);
                  digitalWrite(27, HIGH);
                  digitalWrite(33, HIGH);
                  digitalWrite(25, HIGH);

                  digitalWrite(18, LOW);
                  digitalWrite(19, HIGH);
                  digitalWrite(5, LOW);
                }
             }      
              //cari_arah_timur();
          }
        }
    }
}

void baca_hmc()
{
  /* Get a new sensor event */
sensors_event_t event;
mag.getEvent(&event);
 
// Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
// Calculate heading when the magnetometer is level, then correct for signs of axis.
float heading = atan2(event.magnetic.y, event.magnetic.x);
 
// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
// Find yours here: http://www.magnetic-declination.com/
// Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
float declinationAngle = 0.22;
heading += declinationAngle;
 
// Correct for when signs are reversed.
if(heading < 0)
heading += 2*PI;
 
// Check for wrap due to addition of declination.
if(heading > 2*PI)
heading -= 2*PI;
 
// Convert radians to degrees for readability.
int headingDegrees = heading * 180/M_PI;
 
//Serial.print("Heading (degrees): ");
//Serial.println(headingDegrees);
 lcd.setCursor(13,1);
 lcd.print(headingDegrees);
 lcd.setCursor(11,1);
 lcd.print(tahap);
 arah = headingDegrees;
 //delay(30);
 //lcd.clear();
}
