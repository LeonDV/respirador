#include <SoftwareSerial.h>
#include <Nextion.h>

SoftwareSerial nextion(7, 6);// Nextion TX to pin 2 and RX to pin 3 of Arduino 
Nextion myNextion(nextion, 9600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
int n=0;
int y=0;
void setup() {
  Serial.begin(9600);
  myNextion.init(); // send the initialization commands for Page 0
  delay(1500);
  myNextion.setComponentValue("n6",5);
  
  myNextion.setComponentValue("n3",0);

  //myNextion.setComponentValue("n0",30);
  //myNextion.sendCommand("delay=50");
  //String Hola = "add 8,0,120";
}

void loop() {

funWave(3,0,n);
funWave(4,0,y);

  n=n+1;
  if(n==60){
    y=y+3;
    if(y==30){
      myNextion.setComponentValue("n3",20);
      myNextion.setComponentValue("n4",470);
      int Fr = myNextion.getComponentValue("n2");
      myNextion.setComponentValue("n5",Fr);
       }
      n=0;
    }
}

void funWave(int id, int channel,int y){

  String Wave = "add ";
  Wave += id;
  Wave += ",";
  Wave += channel;
  Wave += ",";
  Wave += y;

  nextion.print(Wave);
  nextion.write(0xff);
  nextion.write(0xff);
  nextion.write(0xff);
  
  }
