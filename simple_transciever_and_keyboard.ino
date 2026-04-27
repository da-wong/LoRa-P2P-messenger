// this is the code for a LoRa texting system (untested)
// i would like to credit all the libary examples for giving me an easy place to steal code from and james bray for introducing me to writing really poor quality notes
// code notes
// the key to change the mode of the module from slave to master may need to be updated to reflect a better key to use
// the code does not current get past the setup stage at the point that says here
// screen is not intergrated into writing bits
// display.println not working
// current failsafes for screen and lora intialisation factored out as i dont have the parts yet

// changelog
// 18/04/26 began change log, determined that there is an issue with display.println, added a failsafe for the keyboard
// 27/04/26 display.println still doesnt wok ill wait till i have to display to test everything. also updated the code t be esp32 compatable 
// 27/06/26 i have just realised i might need to reassign a lot of pin numbers

#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1305.h>
#include <BBQ10Keyboard.h>


//**!!all values are for software SPI on arduino SOFTWARE SPI!!**
// **pin definitions are currently unconfirmed treat all defs as incorrect** YOU BETTER WORK THIS OUT AND CHANGE IT
// i have changed them (i have no clue if it will work)
#define OLED_CLK 27
#define OLED_MOSI 26
#define OLED_CS 25
#define OLED_DC 33
#define OLED_RESET 32 //who knows if we will need it
//**closing the unconfrimed space in code**

Adafruit_SSD1305 display(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS); //this is for sware serial on the screen

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

int slave = 0; // change this to intialise in slave or master
// enum 1 = slave; 1 != master

BBQ10Keyboard keyboard;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LoRa.begin(915E6);// remove when lora attached
  LoRa.setTxPower(22);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(7.8E3);
  LoRa.setCodingRate4(8);
  //if (!LoRa.begin(915E6)) {
    //Serial.println("The LoRa didn't work :(((");
   // while (1);
//  }
  //if (! display.begin(0x3C)){
   // Serial.println("the screen didnt work :(");
    //while (1) yield();
//  }
//  if (!keyboard.begin()) {
//    Serial.println("the keyboard didnt work :(");
//    while (1) yield();
//  }
  display.begin(0x3C); //remove when screen attached 
  display.display(); // show splashscreen
  delay(1000);
  display.clearDisplay();   // clears the screen and buffer
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
//  display.println("Hello, world!");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
//  display.println("3.141592");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.display();
  delay(1000);
  display.clearDisplay();
  keyboard.begin(); // remove later
  keyboard.setBacklight(0.5f);
}

void loop() {
  const BBQ10Keyboard::KeyEvent key = keyboard.keyEvent();  
  int packetsize = LoRa.parsePacket();
  if (key.state == BBQ10Keyboard::StatePress && key.key == '\n' && slave != 1) {   // slave mode
    Serial.println("receiving mode");
    if (packetsize) {
      Serial.println("Recieved Packet: ");
      while (LoRa.available()) {
        Serial.print((char)LoRa.read());
      }
      Serial.print(" with RSSI");
      Serial.println(LoRa.packetRssi());
    }

  }
  if (key.state == BBQ10Keyboard::StatePress && key.key == '\n' && slave == 1) {  // master mode
    slave = 0;
    Serial.println("sending mode");
    String msg = String("");
    if (Serial.available()) {
      Serial.println("Input message here (press enter to send): ");
      if (key.state == BBQ10Keyboard::StatePress && key.key != '\n') {
        String msg = msg + key.key;
      }
      for (unsigned int i = 0; i < msg.length(); i++){
        Serial.print(msg[i] >> 4, HEX);
        Serial.print(msg[i] & 0xF, HEX);
      }      
    }
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();

    Serial.println("message sent :)");
    slave = 1;
    //ideas for screen, do i present the text as the string and update that every time (this cn be done for recieving in slave)
    //or is the better option to sort out the text (for less power intesive maybe a little harder, look into more when not on plane maybe)
  }


}  
