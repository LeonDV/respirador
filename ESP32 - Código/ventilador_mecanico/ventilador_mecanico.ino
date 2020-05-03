// Inclución de modulos
#include <Nextion.h>

// Instanciación de objetos
Nextion myNextion(Serial2, 115200);

// Declaración de variables
// Periodo de muestro
unsigned long t, t0 = 0;
const int Ts = 7500; //Periodo de muestreo en microsegundos

// PWM 
const int pwmFreq = 50;
const int pwmChannel = 0;
const int pwmBits = 12;

// Driver Motor
const int motorIn1 = 27; 
const int motorIn2 = 26;
const int motorEnA = 14; 

// Encoder Motor
const int motorChA = 32; 
const int motorChB = 33;
byte previusState = 0;
int velDir = 0;
float pulsos = 0;

// Motor
float posMotor = 180;
float velMotor = 0;

// Parametros ventilador
const float posInicial = 180;
const float posFinal = 240;
int t1 = 1000; // Tiempos en milisegundos
const int t2 = 150;
int t3 = 1850;
int bpm = 20;
int a = 2;

// Parametros controlador
float ref = 180;
int k = 0;

// Calculo velocidades referencia
float Vs = (posFinal - posInicial)*Ts/(1000*t1);
const float Vb = (posFinal - posInicial)*Ts/(1000*600);

// Calculo de numero de muestras
int Ns = 1000*t1/Ts;
const int Np = 1000*t2/Ts;
int Nb = 1000*t3/Ts;

// Ciclo PWM - Velocidad motor
int pwmMotor = 11375*(posFinal - posInicial)/t1; // Constante 4095*1000/360

// Variables comunicación Nextion
int pwm = 0;
int enable = 0;
int reset = 0;
int pos = 180;
int select = 0;

// Variables control
float error = 0;
//float k = 1;

// Inicialización
void setup() {
  // Declaración de pines ESP32
  // Entradas
  pinMode(motorChA, INPUT);
  pinMode(motorChB, INPUT);
  // Salidas
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorEnA, OUTPUT);
  pinMode(25, OUTPUT);
  // Interrupciones
  attachInterrupt(motorChA, funEncoder, CHANGE);
  attachInterrupt(motorChB, funEncoder, CHANGE);
  
  // Configuración del PWM
  ledcSetup(pwmChannel, pwmFreq, pwmBits);
  ledcAttachPin(motorEnA, pwmChannel);

  // Iniciar puertos seriales
  Serial.begin(115200);
  Serial2.begin(115200);

  // Inicialización pantalla nextion
  myNextion.init();
}

// Ejecución de código
void loop() { 

//t1 = 60000/((1+a)*bpm); 
//t3 = 60000*a/((1+a)*bpm);
//
//Vs = (posFinal - posInicial)*Ts/(1000*t1);
//
//Ns = 1000*t1/Ts;
//Nb = 1000*t3/Ts;
//
//ref = posInicial;
//k = 0;
  
  t = micros();

  if (t < t0){
    t0 = 0;
  }
  
  if (t - t0 >= Ts){ 

    int nexRead = myNextion.getComponentValue("va4");
      
    if (nexRead == 1){
      enable = myNextion.getComponentValue("va0");
      pwm = myNextion.getComponentValue("va1");
      reset = myNextion.getComponentValue("va2");
      pos = myNextion.getComponentValue("va3");
      select = myNextion.getComponentValue("va7");
    }
    
    if (reset == 1){
      stopMotor();
      k = 0;
      ref = 180;
      posMotor = 180;
    }
    
    if (enable == 1){

      k++;
   
      // Control automatico y calculo de referencia
      if (k <= Ns){
        ref = posInicial + Vs*k;
      } else if (k > Ns && k <= Ns + Np){
        ref = posFinal;
      } else if (k > Ns + Np && k <= Ns + Np + Nb){
        if(ref > posInicial){
          ref = posFinal - Vb*(k - (Ns + Np));
        }else{
          ref = posInicial;
        }                   
      } else{
        k = 0;    
        ref = posInicial;
      }
      
      error = ref - posMotor;
           
      //pwm = 3695/50*abs(error) + 400;
      pwm  = 3000*abs(error) + 1420; 

      if (pwm > 4095){
        pwm = 4095;
      }

      if (error > 0.1){
        rotateLeft(pwm);
      }else if (error < -0.1){            
        rotateRight(pwm);
      }else{
        stopMotor();
      }
      

    } else {

      // Control manual por interfaz
      switch (select) {
        case 0: // Control velocidad motor
          ledcWrite(pwmChannel, pwm);
          break;
        case 1: // Control posición motor
          error = pos - posMotor;
          if (error > 1){
            rotateLeft(pwm);
          }else if (error < -1){            
            rotateRight(pwm);
          }else{
            stopMotor();
          }
          break;
        default: // Error de selección
          stopMotor();
          break;
      }
      
    }
    
    velMotor = 21536.25269*pulsos/Ts; // La constante es 60*10^6/2786
    pulsos = 0;
   
    grafSerial();
    
    //dacWrite(25,int(255*posMotor/posFinal));
    dacWrite(25,int(255*ref/posFinal));
  
    t0 = t;
     
  }
  
}

// Funciones

// Control Motor
void rotateRight(int dutyCycle){ // Girar sentido horario
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  ledcWrite(pwmChannel, dutyCycle);  
  }
void rotateLeft(int dutyCycle){ // Girar sentido antihorario
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  ledcWrite(pwmChannel, dutyCycle);  
  }
void stopMotor(){ // Parar
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
}

// Graficar
void grafSerial(){
  Serial.print("Ts = ");
  Serial.print(t-t0);
  Serial.print(" [us]");
  Serial.print("\t");
  Serial.print("PWM = ");
  Serial.print(pwm);
  Serial.print("\t");
  Serial.print("Vel = ");
  Serial.print(-velDir*velMotor);
  Serial.print(" [rpm]");
  Serial.print("\t");
  Serial.print("Pos = ");
  Serial.print(posMotor);
  Serial.print(" [º]");
  Serial.print("\t");
  Serial.print("Ref = ");
  if (enable == 1){
   Serial.print(ref);
  }else{
   Serial.print(pos);
  }   
  Serial.println(" [º]");
}

// Interrupción Enconder
void funEncoder(){

  switch (previusState) {
  case 0:
    if (digitalRead(motorChA) == LOW && digitalRead(motorChB) == HIGH){
      velDir = 1;
      previusState = 1;
    } else if (digitalRead(motorChA) == HIGH && digitalRead(motorChB) == LOW){
      velDir = -1;
      previusState = 2;      
    } else {
      velDir = 0;
    }
    break;
  case 1:
    if (digitalRead(motorChA) == HIGH && digitalRead(motorChB) == HIGH){
      velDir = 1;
      previusState = 3;
    } else if (digitalRead(motorChA) == LOW && digitalRead(motorChB) == LOW){
      velDir = -1;
      previusState = 0;      
    } else {
      velDir = 0;
    }
    break;
  case 2:
    if (digitalRead(motorChA) == LOW && digitalRead(motorChB) == LOW){
      velDir = 1;
      previusState = 0;
    } else if (digitalRead(motorChA) == HIGH && digitalRead(motorChB) == HIGH){
      velDir = -1;
      previusState = 3;      
    } else {
      velDir = 0;
    }
    break;
  case 3:
    if (digitalRead(motorChA) == HIGH && digitalRead(motorChB) == LOW){
      velDir = 1;
      previusState = 2;
    } else if (digitalRead(motorChA) == LOW && digitalRead(motorChB) == HIGH){
      velDir = -1;
      previusState = 1;      
    } else {
      velDir = 0;
    }
    break;
  default:
    velDir = 0;
    break;
  }

  pulsos ++;
  posMotor += -0.1292175162*velDir; // La constante es 360/2786
    
}
