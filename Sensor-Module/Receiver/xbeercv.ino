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
#include <ros.h>
#include <std_msgs/Float32.h>
#include <geometry_msgs/Point.h>
/*
This example is for Series 1 XBee (802.15.4)
Receives either a RX16 or RX64 packet and sets a PWM value based on packet data.
Error led is flashed if an unexpected packet is received
*/



geometry_msgs::Point gyro_data;



XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();
XBeeAddress64 adr64 = XBeeAddress64();
//ZBRxResponse adr16 = ZBRxResponse();

ros::NodeHandle nh;

ros::Publisher chatter("enc", &gyro_data);

int statusLed = 11;
int errorLed = 12;
int dataLed = 10;

uint16_t add=1;



uint8_t option = 0;
uint8_t data = 0;

 union {
      float f;
      byte b[4];
   } stuff;

void setup()

 {

  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);
  pinMode(dataLed,  OUTPUT);
  nh.initNode();
  nh.advertise(chatter);
  // start serial
 // Serial.begin(57600);
 // xbee.setSerial(Serial);
 
}

// continuously reads packets, looking for RX16 or RX64
void loop() {
    
    xbee.readPacket();
    
    if (xbee.getResponse().isAvailable()) {
      // got something
      adr64 = rx64.getRemoteAddress64();
      add = rx16.getRemoteAddress16();
      
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

         if(add==2)
         {
         gyro_data.x=stuff.f;
         gyro_data.y=2;
         }
         else if(add==3)
         {
            gyro_data.x=stuff.f;
            gyro_data.y=3;
         }

         chatter.publish(&gyro_data); 

         //Serial.write(stuff.b[0]);
         //Serial.write(stuff.b[1]);
         //Serial.write(stuff.b[2]);
         //Serial.write(stuff.b[3]);
         //Serial.println("");
         
         //Serial.print(stuff.f);
        }
        
      }
    }
    nh.spinOnce();
}
