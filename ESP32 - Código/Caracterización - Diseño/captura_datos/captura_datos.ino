// Declaración de variables
// Periodo de muestro
unsigned long t, t0 = 0;
const int Ts = 4000; //Periodo de muestreo en microsegundos

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
const float posFinal = 210;
const int t1 = 1000; // Tiempos en milisegundos
const int t2 = 150;
const int t3 = 850;

// Parametros controlador
float ref = 180;
int k = 0;

// Calculo velocidades referencia
 float Vs = (posFinal - posInicial)*Ts/(1000*t1);
 float Vb = (posFinal - posInicial)*Ts/(1000*600);

// Calculo de numero de muestras
int Ns = 1000*t1/Ts;
int Np = 1000*t2/Ts;
int Nb = 1000*t3/Ts;

// Ciclo PWM - Velocidad motor
int pwmMotor = 11375*(posFinal - posInicial)/t1; // Constante 4095*1000/360

// Variables control
float pwm = 0;
float error = 0;
int flag = 0;
int controlDelay = 0;
char iniciar = '0';

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
  // Interrupciones
  attachInterrupt(motorChA, funEncoder, CHANGE);
  attachInterrupt(motorChB, funEncoder, CHANGE);
  
  // Configuración del PWM
  ledcSetup(pwmChannel, pwmFreq, pwmBits);
  ledcAttachPin(motorEnA, pwmChannel);

  // Iniciar puertos seriales
  Serial.begin(115200);

}

// Ejecución de código
void loop() { 
  if(iniciar == '0'){
    noInterrupts();
   while(1){
    if (Serial.available()>0)
      //Serial.print(iniciar);
      iniciar = Serial.read();
      if (iniciar == '1'){
        flag = 0;
        controlDelay = 0;
        pwm = 0;
        interrupts();
        break;
      }
      delay(10);
   }
  }

  t = micros();
  
  if (t < t0){
    t0 = 0;
  }
  
  
  if (t - t0 >= Ts){  

    controlDelay++;

    if (flag == 0 && controlDelay >= 10){
    // Control automatico y calculo de referencia
      k++;
      if (k <= Ns){
        ref = posFinal; 
      } else if (k > Ns && k <= Ns + Np){
        ref = posInicial;
      } else if (k > Ns + Np && k <= Ns + Np + Nb){
        if(ref > posInicial){
          ref = posInicial;
        }else{
          ref = posInicial;
        }               
      } else{
        k = 0;
        flag = 1;
        iniciar = '0';
        ref = posInicial;
      }
        
      error = ref - posMotor;      
      //pwm = 3495/50*abs(error) + 1460;

      pwm = 4095;
  
      if (pwm > 4095){
        pwm = 4095;
      }
  
      if (error > 1){
        rotateLeft(pwm);
      }else if (error < -1){            
        rotateRight(pwm);
      }else{
        stopMotor();
      }
    }  
    
    velMotor = 21536.25269*pulsos/Ts; // La constante es 60*10^6/2786
    pulsos = 0;
   
    grafSerial();
  
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
  Serial.print(flag);
  Serial.print(',');
  Serial.print(t-t0);
  Serial.print(',');
  Serial.print(t);
  Serial.print(',');
  Serial.print(ref);
  Serial.print(',');
  Serial.print(posMotor);
  Serial.print(',');
  Serial.print(-velDir*velMotor); 
  Serial.print(',');
  Serial.println(pwm); 
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
