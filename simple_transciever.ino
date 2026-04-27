// this is the version that only contains transciever code (untested)

#include <LoRa.h>

int slave = 0; // change this to intialise in slave or master

char msg = "the messga is";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  LoRa.setTxPower(22);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(7.8E3);
  LoRa.setCodingRate4(8);
  if (!LoRa.begin(915E6)) {
    Serial.println("The LoRa didn't work :(((");
    while (1);
  }
}

void loop() {
  
  int packetsize = LoRa.parsePacket();
  if (slave == 1) {   // slave mode
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
  if (slave != 1) {  // master mode
    Serial.println("sending mode");
    
    if (Serial.available()) {
      Serial.println("Input message here (press enter to send): ");
      String msg = Serial.readStringuntil('\n');
    }

    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();

    Serial.println("message sent :)");
    slave = 1;
  }
}  