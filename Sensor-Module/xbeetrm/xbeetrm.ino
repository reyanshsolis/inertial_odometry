// Keep the circuit stable on a surface for atleast 10 seconds to set offset
// enc_trans03_r - transmits RAW data from IMU : ax ay az gx gy gz dc_offset 
// LEFT SIDE WHEEL

//SDA=A4
//SCL=A5


#include <Wire.h>  
#include <LSM6.h>
#include <SPI.h>
#include <XBee.h>


LSM6 imu;

long int sampleNum = 1000; 
float dc_offset = 0;
float dc_offset_x = 0;
float dc_offset_y = 0;
float dc_offset_z = 0;

char report[80];


XBee xbee = XBee();

uint8_t payload[] = { 0, 0, 0, 0};
int pin5 = 0;

int statusLed = 13;
int errorLed = 13;

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4189730b);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


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




void setup() {

  delay(5000);
  
  pinMode(statusLed, OUTPUT);
  pinMode(errorLed, OUTPUT);

  Serial.begin(9600);
  xbee.setSerial(Serial);


  Wire.begin();
  Serial.println("Y"); //Code has started
  imu.init();
  if (!imu.init())
  {
    Serial.println("N"); // Failed to detect and initialize IMU!
    while (1);
  }
  
  imu.writeReg(LSM6::CTRL9_XL, 0x38);  // Acc X, Y, Z axes enabled
  imu.writeReg(LSM6::CTRL1_XL, 0x70);  // Acc = 833Hz (High-Performance mode)
  imu.writeReg(LSM6::INT1_CTRL, 0x01); // Acc Data Ready interrupt on INT1
  imu.writeReg(LSM6::CTRL10_C, 0x38);  // Gyro X, Y, Z axes enabled
  imu.writeReg(LSM6::CTRL2_G, 0x7C);   // Gyro = 833Hz (High-Performance mode) and 2000dps
  imu.writeReg(LSM6::INT2_CTRL, 0x02); // Gyro Data Ready interrupt on INT1
  
  for(int n=0;n<sampleNum;n++)
  { 
    imu.writeReg(LSM6::STATUS_REG, 0x0B); //this hex number represents the status register
    imu.read(); 
    snprintf(report, sizeof(report), "A: %6d %6d %6d    G: %6d %6d %6d", imu.a.x, imu.a.y, imu.a.z, imu.g.x, imu.g.y, imu.g.z);
    dc_offset_z += imu.g.z;
    dc_offset_y += imu.g.y;
    dc_offset_x += imu.g.x;
  } 
  
  dc_offset_z =dc_offset_z/sampleNum;  
  dc_offset_y =dc_offset_y/sampleNum;
  dc_offset_x =dc_offset_x/sampleNum;
  //Serial.println(dc_offset); 
  //Serial.print("#"); //indicates start of msg
}

void loop() {   


  //double aix, aiy, aiz;
  float gix, giy, giz, gi;
    
  imu.writeReg(LSM6::STATUS_REG, 0x0B); //this hex number represents the status register
  imu.read();
  snprintf(report, sizeof(report), "A: %6d %6d %6d    G: %6d %6d %6d", imu.a.x, imu.a.y, imu.a.z, imu.g.x, imu.g.y, imu.g.z);

  //aix = imu.a.x;
  //aiy = imu.a.y;
  //aiz = imu.a.z;

/* Conversion factor :   
 * since we are using 2000 degrees/seconds range
 * -2000 maps to a raw value of -32768
 * +2000 maps to a raw value of 32767
 * So, g(degree/sec.)=g(raw)*2000.0/32768.0
 * g(rpm)=g(degree/sec) /6    // rpm = (360degree)/(60sec) = (1/6)(degree/sec)
 * So, g(rpm) = 0.0101725 * g(raw)
 * 
 * for acc. : 
 * since we are using 2G range
 * -2g maps to a raw value of -32768
 * +2g maps to a raw value of 32767
 */

  gix = 0.0101725 * (imu.g.x - dc_offset_x);
  giy = 0.0101725 * (imu.g.y - dc_offset_y);
  giz = 0.0101725 * (imu.g.z - dc_offset_z);
  
  gi=sqrt(sq(gix)+sq(giy)+sq(giz));
//  Serial.print("%");
//  Serial.print(gi);   
//  Serial.print("%");

long int val=int(gi*10000);

  union {
      float f;
      byte b[4];
   } stuff;


payload[0] = byte(val >> 24 & 0xff);//stuff.b[0];
payload[1] = byte(val >> 16 & 0xff);//stuff.b[1];
payload[2] = byte(val >> 8 & 0xff);//stuff.b[2];
payload[3] = byte(val);//stuff.b[3];
//payload[4] = stuff.b[4];
//payload[5] = stuff.b[5];
//payload[6] = stuff.b[6];
//payload[7] = stuff.b[7];
  // break down 10-bit reading into two bytes and place in payload
//  pin5 = val;
//  payload[0] = pin5 >> 8 & 0xff;
//  payload[1] = pin5 & 0xff;
  Serial.print(payload[0]);
  Serial.print(payload[1]);
  Serial.print(payload[2]);
  Serial.print(payload[3]);
  xbee.send(zbTx);

  // flash TX indicator
  flashLed(statusLed, 1, 100);

  // after sending a tx request, we expect a status response
  // wait up to half second for the status response
  if (xbee.readPacket(500)) {
    // got a response!

    // should be a znet tx status              
    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getDeliveryStatus() == SUCCESS) {
        // success.  time to celebrate
        flashLed(statusLed, 5, 50);
        
      } else {
        // the remote XBee did not receive our packet. is it powered on?
        flashLed(errorLed, 3, 500);
       
      }
    }
  } else if (xbee.getResponse().isError()) {
    //nss.print("Error reading packet.  Error code: ");  
    //nss.println(xbee.getResponse().getErrorCode());
  } else {
    // local XBee did not provide a timely TX Status Response -- should not happen
    flashLed(errorLed, 2, 50);
    
  }

//  delay(1000);
}



