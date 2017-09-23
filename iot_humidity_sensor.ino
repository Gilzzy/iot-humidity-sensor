#include <Wire.h>
#include <chibi.h>
#include <LowPower.h>

int temp = 0;
int humid = 0;
float finalTemp = 0.0;
float finalHumid = 0.0;  
byte first = 0;
byte stat = 0;
byte message[100];
byte newline[] = "\n";
char postData[100];
char tempString[100];
char humidString[100];

//Setup function - this is run every time we wakeup
void setup() {
  chibiInit();
  
//  LowPower Mode
  pinMode(13,OUTPUT);
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

}

//Nothing to do in the main loop, we are setup only then sleep
void loop() {
  int i = 0;
  
  //LowPower Mode
  digitalWrite(13,HIGH);
  delay(2000);
  digitalWrite(13,LOW);
  
  Wire.begin();
  Wire.beginTransmission(39);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(1000);
  Wire.requestFrom(39, 4);
  if (4 == Wire.available()) {
    first = Wire.read();
    stat = first & 0xc0;
    if (stat == 0) {
      humid = ((first & 0x3f) << 8) | Wire.read();
      temp = ((Wire.read() << 8) | (Wire.read() & 0xfc)) >> 2;
      finalTemp = (temp / (pow(2, 14) - 2)) * 165 - 40;
      finalHumid = (humid / (pow(2, 14) - 2)) * 100;

      delay(500);
      
      //Serial.print(temp);
      //Serial.print(humid);
      
      dtostrf(finalTemp, 4, 2, tempString);
      dtostrf(finalHumid, 4, 2, humidString);
      //sprintf(postData, "{\"temperature\":%s,\"humidity\":%s}", tempString, humidString);
      sprintf(postData, "T:%s H:%s \n", tempString, humidString);
      
      Serial.print(postData);
      
      chibiTx(BROADCAST_ADDR, (uint8_t*) postData, 21);
      
      LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
      
    }
  }
  
}
