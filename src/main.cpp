#include <Arduino.h>

// the setup function runs once when you press reset or power the board

byte LS; // hecadecimal for the 2 lightsensors

byte IR; // hexadecimal for the 3 infraredsensors

unsigned long USS; // distance in cm for HC-SR04

void LightS(void);
void infraRedS(void);
void setMotors(char left, char right);
long unsigned distanceS(void);
bool laser;

byte PLSR = A0, // light sensor left
     PLSL = A1, // light sensor left
     PIRL = A2, // infrared sensor left
     PIRR = A3, // infrared sensor right
     PIRM = A4, // infrared senor middle
     PECHO = 6, // HC-SR04 output signal
     PTRIG = 5, // HC-SR04 input signal
     PSL = 10, // servo left
     PSR = 11; // servo right

void setup()
{
    //	inputs light sensors
    pinMode(PLSR, INPUT);
    pinMode(PLSL, INPUT);
    //	inputs infrared sensors
    pinMode(PIRL, INPUT);
    pinMode(PIRR, INPUT);
    pinMode(PIRM, INPUT);
    //  ultrasonic sensor
    pinMode(PTRIG, OUTPUT);
    pinMode(PECHO, INPUT);

    Serial.begin(9600);
}
// the loop function runs over and over again until power down or reset
void loop()
{
    Serial.print("hello\n ");
    // set sensor variables
    LightS();
    infraRedS();
    distanceS();
    // if laser is seen set laser to true. does not change when laser is not visible anymore
    laser = (LS || !laser) ? 1 : 0;
    // if LS or laser is greater than 0, 
    if (LS || laser) {
        switch(LS) {
            // if both are 1
            case 0x3:
                break;
            // if right is 1
            case 0x2:
                break;
            //if left is 1
            case 0x1:
                break;
            // if none are 1
            case 0x0: 
                setMotors(-2, 2);
                break;
        }
    }

    else {
        switch (IR) {
            // if left and mid or only left see IR
            case 0x1:
            case 0x3:
            // if right and mid or only right see IR
                break;
            case 0x4:
            case 0x6:
                break;
            // if mid or outer or all see IR
            case 0x5:
            case 0x2:
            case 0x7:
                break;
        }
    }

}

void LightS(void)
{
}

void infraRedS(void)
{
    int L, R, M;
    L = analogRead(PIRL);
    R = analogRead(PIRR);
    M = analogRead(PIRM);

    if (L <= 0) IR |= 1UL << 3;
    else IR &= ~(1 << 3);
    
    if (M <= 0) IR |= 1UL << 2;
    else IR &= ~(1 << 2);
      
    if (R <= 0) IR |= 1UL << 1;
    else IR &= ~(1 << 1);
}

void setMotors(char left, char right)
{
    char L = (255/2) - (255 / 2.0) * left;
    char R = (255/2) - (255 / 2.0) * right;

    analogWrite(PSL, L);
    analogWrite(PSR, R);
}

unsigned long distanceS(void)
{
    digitalWrite(PTRIG, LOW);
    delayMicroseconds(2);
    // send trigger signal
    digitalWrite(PTRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PTRIG, LOW);
    // reveive signal
    unsigned long i = pulseIn(PECHO, HIGH);
    USS = i * 0.034 / 2;
    return USS;
}