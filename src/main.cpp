/*
Capture The Flag RFID Edition: created by Marco Vázquez Madero 

Developed and tested for Arduino Nano ATmega328P (Both Old and New Bootloader)
You can change this in platformio.ini settings, Just add new for the NewBootloader

Wiring diagram:
---------------------------------------------------
      Arduino   |   MFRC522   |  OLED   |  Buzzer |
---------------------------------------------------
---------------------------------------------------
      D5        |             |         |    +    |
---------------------------------------------------      
---------------------------------------------------
      D9        |    RST      |         |         |
---------------------------------------------------
      D10       |    SDA      |         |         |
---------------------------------------------------
      D11       |    MOSI     |         |         |
---------------------------------------------------
      D12       |    MISO     |         |         |
---------------------------------------------------
      D13       |    SCK      |         |         |
---------------------------------------------------
---------------------------------------------------
      A4(SDA)   |             |    SDA  |         |
---------------------------------------------------
      A5(SCL)   |             |    SCL  |         |
---------------------------------------------------
      

*/

//---------------------------------------------------------------------------------
//      Initial Setup
//---------------------------------------------------------------------------------

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define RST_PIN 9 //
#define SS_PIN 10 // Pins for MFRC522 board

LiquidCrystal_I2C lcd1(0x26, 16, 2); // Initiate OLED display
LiquidCrystal_I2C lcd2(0x27, 16, 2); // Initiate OLED display
MFRC522 mfrc522(SS_PIN, RST_PIN);    // Create MFRC522 instance

//---------------------------------------------------------------------------------
//      Variables to change
//---------------------------------------------------------------------------------

char Team1char[] = "A"; // Team 1/A Name
char Team2char[] = "B"; // Team 2/B Name

//---------------------------------------------------------------------------------
//      Declared Functions
//---------------------------------------------------------------------------------

unsigned long TeamXFirstCaptureTime = 0; //Time when the first capture has taken place
unsigned long Team1StartTime = 0;        //Time when Team 1 takes the flag for the first time
unsigned long Team2StartTime = 0;
unsigned long Team1EndTime = 0; //Time when Team 1 lost control of the flag in favor of Team 2
unsigned long Team2EndTime = 0;
unsigned long Team1Time = 0; //Total time Team 1 has had the flag
unsigned long Team2Time = 0;
unsigned long Hours1 = 0;
unsigned long Mins1 = 0;
unsigned long Secs1 = 0;
unsigned long Secs1Flag = 0;
unsigned long Hours2 = 0;
unsigned long Mins2 = 0;
unsigned long Secs2 = 0;
unsigned long Secs2Flag = 0;

unsigned int Team1Captures = 0; //Number of times Team 1 has captured the flag
unsigned int Team2Captures = 0;

bool Team1Flag = 0; //Boolean used to signal which team has the flag
bool Team2Flag = 0;

void CapturingBeep()
{ //Beep when Team 1 captures the flag 

  tone(5, 2000, 500);       //Tone is for PASSIVE buzzers
                            //Use digital Write for ACTIVE buzzers
}

void CheckUID()
{ //MFRC522 void which checks if a new card is present and reads it

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  if (mfrc522.uid.uidByte[0] == 0x5B || //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x6D || //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x56 || //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x5C || //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x0B || //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x51)
  { //Team 1 uids
    Team1Flag = 1;
    Team2Flag = 0;
    Team1Captures++;
    CapturingBeep();
    if (Team1Captures == 1)
    { //El team1captures resta el tiempo
      TeamXFirstCaptureTime = millis();
    }
  }

  if (mfrc522.uid.uidByte[0] == 0xF2 || //Team 2 uids
      mfrc522.uid.uidByte[0] == 0xF1 || //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x7A || //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x89 || //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x04)
  { //Team 2 uids
    Team1Flag = 0;
    Team2Flag = 1;
    Team1Captures++;
    CapturingBeep();
    if (Team1Captures == 1)
    { //Time when the flag is captured
      TeamXFirstCaptureTime = millis();
    }
  }
}

void Team1PointsDisplay(unsigned long TimetoDisplay)
{ //Void to display in the 1st OLED Team 1's points

  Hours1 = (TimetoDisplay / 3600000);
  Mins1 = (TimetoDisplay / 60000 - Hours1 * 60);
  Secs1 = (TimetoDisplay / 1000 - Mins1 * 60 - Hours1 * 3600);
  lcd1.setCursor(1, 1);
  lcd1.print(Hours1);
  lcd1.setCursor(3, 1);
  lcd1.print(":");
  lcd1.setCursor(5, 1);
  lcd1.print((Mins1 / 10) % 10);
  lcd1.setCursor(7, 1);
  lcd1.print(Mins1 % 10);
  lcd1.setCursor(9, 1);
  lcd1.print(":");
  lcd1.setCursor(11, 1);
  lcd1.print((Secs1 / 10) % 10);
  lcd1.setCursor(13, 1);
  lcd1.print(Secs1 % 10);

  lcd1.setCursor(4, 0);
  lcd1.print("Equipo 1");
}

void Team2PointsDisplay(unsigned long TimetoDisplay)
{ //Void to display in the 2nd OLED Team 2's points

  Hours2 = (TimetoDisplay / 3600000);
  Mins2 = (TimetoDisplay / 60000 - Hours2 * 60);
  Secs2 = (TimetoDisplay / 1000 - Mins2 * 60 - Hours2 * 3600);

  lcd2.setCursor(1, 1);
  lcd2.print(Hours2);
  lcd2.setCursor(3, 1);
  lcd2.print(":");
  lcd2.setCursor(5, 1);
  lcd2.print((Mins2 / 10) % 10);
  lcd2.setCursor(7, 1);
  lcd2.print(Mins2 % 10);
  lcd2.setCursor(9, 1);
  lcd2.print(":");
  lcd2.setCursor(11, 1);
  lcd2.print((Secs2 / 10) % 10);
  lcd2.setCursor(13, 1);
  lcd2.print(Secs2 % 10);

  lcd2.setCursor(4, 0);
  lcd2.print("Equipo 2");

}

//---------------------------------------------------------------------------------
//      Program setup
//---------------------------------------------------------------------------------

void setup()
{

  Serial.begin(115200); // Initialize serial communications with the PC for debugging
  lcd1.init();          // Initialize lcds
  lcd2.init();
  lcd1.backlight();
  lcd2.backlight();
  lcd1.clear();
  lcd2.clear();
  //while (!Serial);		                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  delay(4);           // Optional delay. Some board do need more time after init to be ready, see Readme
}

//---------------------------------------------------------------------------------
//      Program
//---------------------------------------------------------------------------------

void loop()
{

  Team1PointsDisplay(Team1Time);
  Team2PointsDisplay(Team2Time);

  CheckUID();

  if (Team1Flag == 1)
  {
    Team1Time = millis() - TeamXFirstCaptureTime - Team2Time;
  }

  if (Team2Flag == 1)
  {
    Team2Time = millis() - TeamXFirstCaptureTime - Team1Time;
  }
}