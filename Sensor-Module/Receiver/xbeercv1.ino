/*
 * rosserial Publisher Example
 * Prints "hello world!"
 */

#include <ros.h>
#include <std_msgs/String.h>
#include <XBee.h>
#include <geometry_msgs/Point.h>

ros::NodeHandle  nh;

geometry_msgs::Point gyro_data;

ros::Publisher chatter("enc", &gyro_data);

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();
Rx64Response rx64 = Rx64Response();
XBeeAddress64 adr64 = XBeeAddress64();
//ZBRxResponse adr16 = ZBRxResponse();

char hello[13] = "hello world!";

uint16_t add=1;
uint8_t option = 0;
uint8_t data = 0;

int z0,z1,z2,z3,z4,z5;

union {
      float f=0;
      byte b[4];
   } stuff;


void setup()
{
  nh.initNode();
  nh.advertise(chatter);

  Serial1.begin(9600);
  xbee.setSerial(Serial1);
}

void loop()
{
   

  xbee.readPacket();
      if (xbee.getResponse().isAvailable()) {
      // got something
      adr64 = rx64.getRemoteAddress64();
      add = rx16.getRemoteAddress16();
     
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
       
        // got a rx packet
        
        if (xbee.getResponse().getApiId() == RX_16_RESPONSE) 
        {
          xbee.getResponse().getRx16Response(rx16);
          option = rx16.getOption();
          //data = rx16.getData(0);
          
          z0=rx16.getData(0)-'0';
          z1=rx16.getData(1)-'0';
          z2=rx16.getData(2)-'0';
          z3=rx16.getData(3)-'0';
          z4=rx16.getData(4)-'0';
          z5=rx16.getData(5)-'0';
          
//          stuff.b[4] = rx16.getData(4);
//          stuff.b[5] = rx16.getData(5);
//          stuff.b[6] = rx16.getData(6);
//          stuff.b[7] = rx16.getData(7);

            if(add==2)
            {
              gyro_data.y=2;
            }
            else if(add==3)
            {
              gyro_data.y=3;
            }


         }
         else {
                xbee.getResponse().getRx64Response(rx64);
         option = rx64.getOption();
          data = rx64.getData(0);

        
        }
        
      }
    }

  
  gyro_data.x=z0*1000 + z1*100 + z2*10 + z3 + z4*0.1 + z5*0.01  ;//(z0<<24) + (z1<<16) + (z2<<8) + (z3);
  Serial.println(gyro_data.x);
  chatter.publish(&gyro_data);  
  nh.spinOnce();
  
}
