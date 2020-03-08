//===========================================================================//
//                                                                           //
//  Desc:    Projeto esgrima                                                 //
//  Dev:     Marcelo Nunes / Mariana Das Merces                              //
//  Date:    Mar  2020                                                       //
//===========================================================================//

//============
// #defines
//============
//TODO: set up debug levels correctly
#define DEBUG 0
#define BUZZERTIME  1000  // Tempo em que a campainha será mantida após um toque (ms)
#define LIGHTTIME   3000  // Tempo em que a luz ficará ligada após um toque (ms)
#define BAUDRATE   57600  // Taxa de transmissão da interface de depuração serial

//============
// Pin Setup
//============
const uint8_t onTargetA  = 11;    // Pino da luz A
const uint8_t onTargetB  = 12;    // Pino da luz B

const uint8_t groundPinA = A0;    // Guarda/Terra    A pin - Analog
const uint8_t lamePinA   = A1;    // Retorno da arma A pin - Analog
const uint8_t weaponPinA = A2;    // Arma            A pin - Analog
const uint8_t weaponPinB = A3;    // Arma            B pin - Analog
const uint8_t lamePinB   = A4;    // Retorno da arma B pin - Analog
const uint8_t groundPinB = A5;    // Guarda/Terra    B pin - Analog
     
const uint8_t modePin    =  7;        // Tomada pin 0 (digital pin 2)
const uint8_t buzzerPin  =  3;        // campainha
const uint8_t modeLeds[] = {4, 5, 6}; // Pinos do LED que indica a arma selecionada {f e s}

//=========================
// Valores default da arma
//=========================
int weaponA = 0;
int weaponB = 0;
int lameA   = 0;
int lameB   = 0;
int groundA = 0;
int groundB = 0;

long depressAtime = 0;
long depressBtime = 0;
bool lockedOut    = false;

//==========================
// Intervalo no toque
//==========================
//Florete => foil
//Espada => epee
//Sabre => sabre
// O intervalo de toque no florete é de 300ms +/-25ms
// O intervalo de toque no espada é de 45ms +/-5ms (40ms -> 50ms)
// O intervalo de toque no sabre é de 120ms +/-10ms
//                         foil   epee   sabre
const long lockout [] = {300000,  45000, 120000};
const long depress [] = { 14000,   2000,   1000};

//=================
// Armas
//=================
const uint8_t FOIL_MODE  = 0;
const uint8_t EPEE_MODE  = 1;
const uint8_t SABRE_MODE = 2;

uint8_t currentMode = EPEE_MODE;


//=========
// Estado
//=========
boolean depressedA  = false;
boolean depressedB  = false;
boolean hitOnTargA  = false;
boolean hitOffTargA = false;
boolean hitOnTargB  = false;
boolean hitOffTargB = false;




//================
// Configurações
//================
void setup() {
   pinMode(modeLeds[0], OUTPUT);
   pinMode(modeLeds[1], OUTPUT);
   pinMode(modeLeds[2], OUTPUT);

   // Configuração de saida
   pinMode(onTargetA,  OUTPUT);
   pinMode(onTargetB,  OUTPUT);
   pinMode(buzzerPin,  OUTPUT);

   digitalWrite(modeLeds[currentMode], HIGH);

   Serial.begin(BAUDRATE);
   Serial.println("3 Weapon Scoring Box");
   Serial.println("====================");
   Serial.print  ("Mode : ");
   Serial.println(currentMode);

   resetValues();
}

void loop() {
   while(1) {
      checkIfModeChanged();
      // read analog pins
      weaponA = analogRead(weaponPinA);
      weaponB = analogRead(weaponPinB);
      lameA   = analogRead(lamePinA);
      lameB   = analogRead(lamePinB);

      Serial.print("A = ");
      Serial.print(weaponA);
      Serial.print(" - ");
      Serial.print(lameA);
      Serial.print(" B = ");
      Serial.print(weaponB);
      Serial.print(" - ");
      Serial.println(lameB);

      signalHits();
      if      (currentMode == FOIL_MODE)
         foil();
      else if (currentMode == EPEE_MODE)
         epee();
      else if (currentMode == SABRE_MODE)
         sabre();
   }
}


//============================
// Mudar a arma
//============================
void setModeLeds() {
   for (uint8_t i = 0; i < 3; i++) {
      digitalWrite(modeLeds[i], LOW);
   }
   digitalWrite(modeLeds[currentMode], HIGH);
}


//========================
// Run when mode changed
//========================
void checkIfModeChanged() {
  if(digitalRead(modePin)){
    if(currentMode == 2){
      currentMode = 0;
    }else{
      currentMode++;
    }
    delay(200);
  }
  setModeLeds();
}

void foil() {
  // Não foi concluido
}


//===================
// Main - modulo da espada (epee)
//===================
void epee() {
   long now = micros();
   if ((hitOnTargA && (depressAtime + lockout[1] < now)) || (hitOnTargB && (depressBtime + lockout[1] < now))) {
      lockedOut = true;
   }

   // Arma - A
   //  Se o esgrimista A ainda não marcou o timeout do toque (intervalo de verificação do toque simultâneo)
   if (hitOnTargA == false) {
      // Verifica se o valor da arma e o retorno estão dentro desse range
      if (400 < weaponA && weaponA < 600 && 400 < lameA && lameA < 600) {
         //Marcar o momento do inicio do toque
         if (!depressedA) {
            depressAtime = micros();
            depressedA   = true;
         } else {
            // Após o intervalo de verificação do toque simultâneo acender as luzes
            if (depressAtime + depress[1] <= micros()) {
               hitOnTargA = true;
            }
         }
      } else {
         // Zero os valores sem não houve toque
         if (depressedA == true) {
            depressAtime = 0;
            depressedA   = 0;
         }
      }
   }

   // Arma - B
   //  Se o esgrimista B ainda não marcou o timeout do toque (intervalo de verificação do toque simultâneo)
   if (hitOnTargB == false) {
      // Verifica se o valor da arma e o retorno estão dentro desse range
      if (400 < weaponB && weaponB < 600 && 400 < lameB && lameB < 600) {
        //Marcar o momento do inicio do toque
         if (!depressedB) {
            depressBtime = micros();
            depressedB   = true;
         } else {
            // Após o intervalo de verificação do toque simultâneo acender as luzes
            if (depressBtime + depress[1] <= micros()) {
               hitOnTargB = true;
            }
         }
      } else {
         // Zero os valores sem não houve toque
         if (depressedB == true) {
            depressBtime = 0;
            depressedB   = 0;
         }
      }
   }
}


void sabre() {
  // Não foi concluido
}


//==============
// Sinalizar o toque
//==============
void signalHits() {
   if (lockedOut) {
      digitalWrite(onTargetA,  hitOnTargA);
      digitalWrite(onTargetB,  hitOnTargB);
      digitalWrite(buzzerPin,  HIGH);
#ifdef DEBUG
      String serData = String("hitOnTargA  : ") + hitOnTargA  + "\n"
                            + "hitOffTargA : "  + hitOffTargA + "\n"
                            + "hitOffTargB : "  + hitOffTargB + "\n"
                            + "hitOnTargB  : "  + hitOnTargB  + "\n"
                            + "Locked Out  : "  + lockedOut   + "\n";
      Serial.println(serData);
#endif
      resetValues();
   }
}


//======================
// Zerar todos os valores
//======================
void resetValues() {
   delay(BUZZERTIME);             // Aguarda antes de desligar a campanhia
   digitalWrite(buzzerPin,  LOW);
   delay(LIGHTTIME-BUZZERTIME);   // Aguarda antes de desligar a(s) luz(es)
   digitalWrite(onTargetA,  LOW);
   digitalWrite(onTargetB,  LOW);

   lockedOut    = false;
   depressAtime = 0;
   depressedA   = false;
   depressBtime = 0;
   depressedB   = false;

   hitOnTargA  = false;
   hitOffTargA = false;
   hitOnTargB  = false;
   hitOffTargB = false;

   delay(100);
}
