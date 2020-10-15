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

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
//#include <Adafruit_I2CDevice.h>    // Include this library if it doesnt compile correctly
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN         9          // 
#define SS_PIN          10         // Pins for MFRC522 board
#define OLED_RESET      4          // 

Adafruit_SSD1306 display1(OLED_RESET);   // Initiate OLED display
Adafruit_SSD1306 display2(OLED_RESET);   // Initiate OLED display
MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance

//---------------------------------------------------------------------------------
//      Variables to change
//---------------------------------------------------------------------------------

char Team1char[] = "A";          // Team 1/A Name 
char Team2char[] = "B";          // Team 2/B Name
int Team1Centering = 60;         // Add or decrease to center Team 1's text in OLED screen
int Team2Centering = 60; 

//---------------------------------------------------------------------------------
//      Declared Functions
//---------------------------------------------------------------------------------

unsigned long TeamXFirstCaptureTime = 0;        //Time when the first capture has taken place
unsigned long Team1StartTime = 0;               //Time when Team 1 takes the flag for the first time
unsigned long Team2StartTime = 0;
unsigned long Team1EndTime = 0;                 //Time when Team 1 lost control of the flag in favor of Team 2 
unsigned long Team2EndTime = 0;
unsigned long Team1Time = 0;                    //Total time Team 1 has had the flag
unsigned long Team2Time = 0;

unsigned int Team1Captures = 0;                 //Number of times Team 1 has captured the flag
unsigned int Team2Captures = 0;

bool Team1Flag = 0;                             //Boolean used to signal which team has the flag
bool Team2Flag = 0;


void CapturingDisplay1(){                       //OLED filling bar animation when Team 1 captures the flag
  for (int i = 0; i < 5; i++){
    digitalWrite(5, HIGH);
    display1.clearDisplay();
    display1.drawRect(20, 8, 80, 20, WHITE);
    display1.fillRect(22, 10, i*16, 16, WHITE);
    display1.display();
  }
  digitalWrite(5, LOW);
}

void CapturingDisplay2(){                      //OLED filling bar animation when Team 2 captures the flag
  for (int i = 0; i < 5; i++){
    digitalWrite(5, HIGH);
    display2.clearDisplay();
    display2.drawRect(20, 8, 80, 20, WHITE);
    display2.fillRect(22, 10, i*16, 16, WHITE);
    display2.display();
  }
  digitalWrite(5, LOW);
}
/* 
void Time1toChar(unsigned long TimetoDisplay){
  display2.setTextSize(2); 
  display2.setTextColor(WHITE);
  display2.setCursor(10,14); 
}
 */
void Time2toChar(){

}

void CheckUID(){        //MFRC522 void which checks if a new card is present and reads it

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  if (mfrc522.uid.uidByte[0] == 0x5B ||                   //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x6D ||                   //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x56 ||                   //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x5C ||                   //Team 1 uids
      mfrc522.uid.uidByte[0] == 0x51) {                   //Team 1 uids
        Team1Flag = 1;
        Team2Flag = 0;        
        Team1Captures++;
        CapturingDisplay1();
        if (Team1Captures == 1){                //El team1captures resta el tiempo
          TeamXFirstCaptureTime = millis();
        }
      }
  
  if (mfrc522.uid.uidByte[0] == 0xF2 ||                   //Team 2 uids
      mfrc522.uid.uidByte[0] == 0xF1 ||                   //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x7A ||                   //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x89 ||                   //Team 2 uids
      mfrc522.uid.uidByte[0] == 0x04) {                   //Team 2 uids
        Team1Flag = 0;
        Team2Flag = 1;
        Team1Captures++;
        CapturingDisplay2();
        if (Team1Captures == 1){                          //Time when the flag is captured
          TeamXFirstCaptureTime = millis();
        }
      }
  
}

void Team1Display(){                           //Void to display in the 1st OLED Team 1's name
  display1.setTextSize(1);
  display1.setTextColor(WHITE);
  display1.setCursor(Team1Centering,0);
  display1.println(Team1char);                 //Text to display
}

void Team2Display(){                           //Void to display in the 2nd OLED Team 2's name
  display2.setTextSize(1);
  display2.setTextColor(WHITE);
  display2.setCursor(Team2Centering,0);
  display2.println(Team2char);                 //Text to display
}

void Team1PointsDisplay(unsigned long TimetoDisplay){   //Void to display in the 1st OLED Team 1's points
  display1.setTextSize(2); 
  display1.setTextColor(WHITE);
  unsigned long Hours1 = (TimetoDisplay/3600000);
  unsigned long Mins1 = (TimetoDisplay/60000 - Hours1*60);
  unsigned long Secs1 = (TimetoDisplay/1000 - Mins1*60 - Hours1*3600);
  display1.setCursor(20,14); 
  display1.println(Hours1);
  display1.setCursor(34,14);
  display1.println(":");
  display1.setCursor(46,14);
  display1.println((Mins1/10)%10);
  display1.setCursor(60,14);
  display1.println(Mins1%10);
  display1.setCursor(74,14);
  display1.println(":");
  display1.setCursor(86,14);
  display1.println((Secs1/10)%10);
  display1.setCursor(100,14);
  display1.println(Secs1%10);

}

void Team2PointsDisplay(unsigned long TimetoDisplay){   //Void to display in the 2nd OLED Team 2's points
  display2.setTextSize(2); 
  display2.setTextColor(WHITE);
  unsigned long Hours2 = (TimetoDisplay/3600000);
  unsigned long Mins2 = (TimetoDisplay/60000 - Hours2*60);
  unsigned long Secs2 = (TimetoDisplay/1000 - Mins2*60 - Hours2*3600);
  display2.setCursor(20,14); 
  display2.println(Hours2);
  display2.setCursor(34,14);
  display2.println(":");
  display2.setCursor(46,14);
  display2.println((Mins2/10)%10);
  display2.setCursor(60,14);
  display2.println(Mins2%10);
  display2.setCursor(74,14);
  display2.println(":");
  display2.setCursor(86,14);
  display2.println((Secs2/10)%10);
  display2.setCursor(100,14);
  display2.println(Secs2%10);
}



//---------------------------------------------------------------------------------
//      Program setup
//---------------------------------------------------------------------------------

void setup() {
  pinMode(5, OUTPUT);
	Serial.begin(9600);		                // Initialize serial communications with the PC for debugging
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C address for the FIRST OLED screen  TEAM 1 A
  display2.begin(SSD1306_SWITCHCAPVCC, 0x3D); // I2C address for the SECOND OLED screen  TEAM 2 B
	while (!Serial);		                  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			                    // Init SPI bus
	mfrc522.PCD_Init();		                // Init MFRC522
	delay(4);				                      // Optional delay. Some board do need more time after init to be ready, see Readme
  //display1.clearDisplay();              // Clears the OLED's RAM
  //display2.clearDisplay();


}

//---------------------------------------------------------------------------------
//      Program
//---------------------------------------------------------------------------------

void loop() {

  display1.clearDisplay(); 
  Team1Display();
  Team1PointsDisplay(Team1Time);     
  display1.display();

  display2.clearDisplay(); 
  Team2Display();
  Team2PointsDisplay(Team2Time);     
  display2.display(); 

  CheckUID();

  if (Team1Flag == 1){
    Team1Time = millis() - TeamXFirstCaptureTime - Team2Time; 
  }
  
  if (Team2Flag == 1){
    Team2Time = millis() - TeamXFirstCaptureTime - Team1Time;  
  }
}