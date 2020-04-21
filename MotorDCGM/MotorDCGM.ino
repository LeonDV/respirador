// Tiempos

volatile int Tiempotrasc;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

unsigned long tiempo1 = 600;
unsigned long tiempo2 = 200;
unsigned long tiempo3 = 300;
unsigned long tiempo4 = 500;


unsigned long tiempoSegundos = 0;
unsigned long tiemramp= 0;
unsigned long tiemramp2= 0;

int rampa1[]= {110, 120, 125, 130, 135, 140, 145, 150, 150, 150};
int rampa2[]= {150, 105, 150, 145, 140, 135, 130, 125, 120, 110};
int i=0;
int j=0;

// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

//set movimiento inicial
int adelante= 1;
int atras=0;

// Setting PWM properties
const int freq = 500;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 150;


void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  Tiempotrasc++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}

void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  Serial.begin(115200);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}


void SetMotVeladelante(int dutyCycle){
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH); 
  ledcWrite(pwmChannel, dutyCycle);   
  }

void SetMotVelatras(int dutyCycle){
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 
  ledcWrite(pwmChannel, dutyCycle);   
  }
void Stop(){
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
}
void loop() {
  int sensorValue = analogRead(A0);
  Serial.print(Tiempotrasc);
  Serial.print("\t");
  Serial.println(sensorValue);
 
  if(Tiempotrasc <tiempo1){
//     if(Tiempotrasc<tiemramp+100 && i<=10){
//     tiemramp= Tiempotrasc; 
//     SetMotVeladelante(rampa1[i]);
//     i++;
//  }
  SetMotVeladelante(150);
  }
  else if(Tiempotrasc > tiempo1 && Tiempotrasc<tiempo1+tiempo2){
      Stop();
  }
  else if(Tiempotrasc > tiempo1+tiempo2 && Tiempotrasc<tiempo1+tiempo2+tiempo3){
//    if(Tiempotrasc<tiemramp2+100 && j<=10){
//     tiemramp2= Tiempotrasc; 
//     SetMotVelatras(rampa2[j]);
//     j++;
//  }
     SetMotVelatras(110);
  }
  else if(Tiempotrasc > tiempo1+tiempo2+tiempo3 && Tiempotrasc<tiempo1+tiempo2+tiempo3+tiempo4){
      Stop();
  }
 
   else if(Tiempotrasc >tiempo1+tiempo2+tiempo3+tiempo4){
   Tiempotrasc=0;
   }
}
