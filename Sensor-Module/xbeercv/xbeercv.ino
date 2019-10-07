/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <XBee.h>

/*
This example is for Series 1 XBee (802.15.4)
Receives either a RX16 or RX64 packet and sets a PWM value based on packet data.
Error led is flashed if an unexpected packet is received
*/

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();
XBeeAddress64 adr64 = XBeeAddress64();
//ZBRxResponse adr16 = ZBRxResponse();

int statusLed = 11;
int errorLed = 12;
int dataLed = 10;

uint16_t add=1;

uint8_t option = 0;
uint8_t data = 0;

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

 union {
      float f;
      byte b[4];
   } stuff;

void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);
  
  // start serial
  Serial.begin(9600);
  xbee.setSerial(Serial);
  
  flashLed(statusLed, 3, 50);
}

// continuously reads packets, looking for RX16 or RX64
void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      adr64 = rx64.getRemoteAddress64();
      add = rx16.getRemoteAddress16();
      Serial.print("@"); 
      Serial.println(add);
      Serial.println(adr64.getLsb());
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
        // got a rx packet
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
                xbee.getResponse().getRx16Response(rx16);
        	option = rx16.getOption();
        	//data = rx16.getData(0);
          stuff.b[0] = rx16.getData(0);
          stuff.b[1] = rx16.getData(1);
          stuff.b[2] = rx16.getData(2);
          stuff.b[3] = rx16.getData(3);
//          stuff.b[4] = rx16.getData(4);
//          stuff.b[5] = rx16.getData(5);
//          stuff.b[6] = rx16.getData(6);
//          stuff.b[7] = rx16.getData(7);
         Serial.write(stuff.b[0]);
         Serial.write(stuff.b[1]);
         Serial.write(stuff.b[2]);
         Serial.write(stuff.b[3]);
         Serial.println("");
         //Serial.print(stuff.f);
        } else {
                xbee.getResponse().getRx64Response(rx64);
        	option = rx64.getOption();
        	//data = rx64.getData(0);
          stuff.b[0] = rx64.getData(0);
          stuff.b[1] = rx64.getData(1);
          stuff.b[2] = rx64.getData(2);
          stuff.b[3] = rx64.getData(3);
//          stuff.b[4] = rx64.getData(4);
//          stuff.b[5] = rx64.getData(5);
//          stuff.b[6] = rx64.getData(6);
//          stuff.b[7] = rx64.getData(7);
         Serial.write(stuff.b[0]);
         Serial.write(stuff.b[1]);
         Serial.write(stuff.b[2]);
         Serial.write(stuff.b[3]);
         Serial.println("");
         //Serial.print(stuff.f);
        }
        
        // TODO check option, rssi bytes    
        flashLed(statusLed, 1, 10);
        
        // set dataLed PWM to value of the first byte in the data
        analogWrite(dataLed, data);
      } else {
      	// not something we were expecting
        flashLed(errorLed, 1, 25);    
      }
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
    } 
}
