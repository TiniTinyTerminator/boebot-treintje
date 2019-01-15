#include <Arduino.h>

// the setup function runs once when you press reset or power the board

char LS; // hecadecimal for the 2 lightsensors
char IR; // hexadecimal for the 3 infraredsensors
char LEDS;
unsigned long USS; // distance in cm for HC-SR04

// declare functions
void LightS(void);
void infraRedS(void);
void setMotors(signed char left, signed char right);
void distanceS(void);
bool laser;

byte PLIRL = 13, // LED left IR-sensor (LEDS << 0)
    PLIRR = 2,   // LED Right IR-sensor (LEDS << 1)
    PLERR = 8,   // LED ERROR (LEDS << 2)
    PLLSR = 7,   // LED Right Light-sensor (LEDS << 3)
    PLLSL = 3,   // LED Left Light-sensor (LEDS << 4)
    PLLSM = 4,   // LED Middle Light-sensor (LEDS << 5)
    PLUSS = 12,  // LED Ultasoon-sensor (LEDS << 6)
    PLSR = A0,   // light sensor left (LS << 0)
    PLSM = A1,   // light sensor middle (LS << 1)
    PLSL = A2,   // light sensor right (LS << 2)
    PIRL = A3,   // infrared sensor left (IR << 0)
    PIRR = A4,   // infrared sensor right (IR << 1)
    PECHO = 6,   // HC-SR04 output signal
    PTRIG = 5,   // HC-SR04 input signal
    PSL = 10,    // servo left
    PSR = 11;    // servo right

void setup()
{
    //	set inputs light sensors
    pinMode(PLSR, INPUT);
    pinMode(PLSL, INPUT);
    pinMode(PLSM, INPUT);
    //	set inputs infrared sensors
    pinMode(PIRL, INPUT);
    pinMode(PIRR, INPUT);
    //  set ultrasonic sensor
    pinMode(PTRIG, OUTPUT);
    pinMode(PECHO, INPUT);
    // set servo output
    pinMode(PSL, OUTPUT);
    pinMode(PSR, OUTPUT);
    // set LEDS
    pinMode(PLIRL, OUTPUT);
    pinMode(PLIRR, OUTPUT);
    pinMode(PLERR, OUTPUT);
    pinMode(PLLSR, OUTPUT);
    pinMode(PLLSL, OUTPUT);
    pinMode(PLLSM, OUTPUT);
    pinMode(PLUSS, OUTPUT);
    Serial.begin(9600);
}
// the loop function runs over and over again until power down or reset
void loop()
{
    // set sensor variables

    LightS();
    infraRedS();
    distanceS();
    delay(100);

    Serial.print(LS, BIN);
    Serial.print("\n");
    Serial.print(USS, DEC);
    Serial.print("\n");
    Serial.print(IR, BIN);
    Serial.print("\n\n");

    // // if laser is seen SET variable laser. does not change when laser is not visible anymore
    // laser = (LS || laser) ? 1 : 0;
    // // if LS or laser is greater than 0
    // if (laser)
    // {
    // if distance is longer than 5 cm
    // if (USS >= 7)
    // {
    //     switch (LS)
    //     {
    //         // if left and mid or only left see the laser
    //     case 0x1:
    //     case 0x3:
    //         if (USS <= 10)
    //             setMotors(1, 0);
    //         else
    //             setMotors(2, 1);
    //         break;

    //         // if right and mid or only right see the laser
    //     case 0x4:
    //     case 0x6:
    //         if (USS <= 10)
    //             setMotors(0, 2);
    //         else
    //             setMotors(1, 2);
    //         break;

    //         // if mid or outer or all see the laser
    //     case 0x5:
    //     case 0x2:
    //     case 0x7:
    //         if (USS <= 10)
    //             setMotors(1, 1);
    //         else
    //             setMotors(2, 2);
    //         break;

    //         // if laser disappeard
    //     case 0x0:

    //         break;
    //     }
    // }
    // // if distance is closer then 5cm
    // else
    // {
    //     // stop for a bit to let the leader get some distance
    //     setMotors(0, 0);
    // }
    // }

    // else
    // {
    //     if (USS >= 10)
    //     {
    //         switch (IR)
    //         {
    //         // if both are 1
    //         case 0x3:
    //             if (USS <= 8)
    //                 setMotors(1, 1);
    //             else
    //                 setMotors(2, 2);
    //             break;
    //         // if right is 1
    //         case 0x2:
    //             if (USS <= 8)
    //                 setMotors(1, 1);
    //             else
    //                 setMotors(2, 2);
    //             break;
    //         //if left is 1
    //         case 0x1:
    //             if (USS <= 8)
    //                 setMotors(1, 1);
    //             else
    //                 setMotors(2, 2);
    //             break;
    //         // if none are 1
    //         case 0x0:
    //             setMotors(-2, 2);
    //             break;
    //         }
    //     }
    //     else
    //     {
    //         setMotors(0, 0);
    //     }
    // }
}

void infraRedS(void)
{
    // read analog signals for IR sensors
    int L, R;
    L = pulseIn(PIRL, HIGH);
    R = pulseIn(PIRR, HIGH);

    if (L)
        // set 2th bit
        IR |= 1UL << 1;
    else
        // unset 2th bit
        IR &= ~(1 << 1);

    if (R)
        // set 1st bit
        IR |= 1UL << 0;
    else
        // unset 1st bit
        IR &= ~(1 << 0);
}

void LightS(void)
{
    int L, R, M;
    L = analogRead(PLSL);
    R = analogRead(PLSR);
    M = analogRead(PLSM);

    if (L > 100)
        LS |= 1UL << 2;
    else
        LS &= ~(1 << 2);

    if (M > 400)
        LS |= 1UL << 1;
    else
        LS &= ~(1 << 1);

    if (R > 100)
        LS |= 1UL << 0;
    else
        LS &= ~(1 << 0);
}

void setMotors(signed char left = 0, signed char right = 0)
{

    switch (right)
    {
    case 2:
        analogWrite(PSR, 200);
        break;
    case 1:
        analogWrite(PSR, 140);
        break;
    case -1:
        analogWrite(PSR, 125);
        break;
    case -2:
        analogWrite(PSR, 100);
        break;
    default:
        analogWrite(PSR, 127);
        break;
    }
    switch (left)
    {
    case 2:
        analogWrite(PSL, 100);
        break;
    case 1:
        analogWrite(PSL, 125);
        break;
    case -1:
        analogWrite(PSL, 137);
        break;
    case -2:
        analogWrite(PSL, 200);
        break;
    default:
        analogWrite(PSL, 127);
        break;
    }
}

void distanceS(void)
{
    // send trigger signal
    digitalWrite(PTRIG, HIGH);
    delay(10);
    digitalWrite(PTRIG, LOW);
    // reveive signal
    unsigned long i = pulseInLong(PECHO, HIGH);
    if (i * 0.034 / 2 > 0 && i * 0.034 / 2 < 250)
        USS = i * 0.034 / 2;
}

void setLEDS()
{
    switch (IR)
    {
    case 0x1: // left IR
        digitalWrite(PLIRL ,HIGH);
        digitalWrite(PLIRR, LOW);
        break;
    case 0x2: // right IR
        digitalWrite(PLIRR, HIGH);
        digitalWrite(PLIRL, LOW);
        break;
    case 0x3:
        digitalWrite(PLIRL, HIGH);
        digitalWrite(PLIRR, HIGH);
        break;
    default:
        digitalWrite(PLIRL, LOW);
        digitalWrite(PLIRR, LOW);
    }

    // switch (LS)
    // {
    // case 0x1: // left LS
    //     digitalWrite(PLLSL, HIGH);
    //     digitalWrite(PLLSM, LOW);;
    //     digitalWrite(PLLSM, LOW);
    //     break;
    // case 0x2: // Middle LS
    //     digitalWrite(PLLSR, HIGH);
    //     digitalWrite(PLLSL, LOW);
    //     digitalWrite(PLLSM, LOW);
    //     break;
    // case 0x3:
    //     digitalWrite(PLLSL, HIGH);
    //     digitalWrite(PLLSR, HIGH);
    //     digitalWrite(PLLSM, LOW);
    //     break;
    // case 0x4: // right LS
    //     digitalWrite(PLLSR, HIGH);
    //     digitalWrite(PLLSL, LOW);
    //     digitalWrite(PLLSM, LOW);
    //     break;
    // case 0x5:
    //     digitalWrite(PLLSL, HIGH);
    //     digitalWrite(PLLSM, HIGH);
    //     digitalWrite(PLLSM, LOW);;
    //     break;
    // case 0x6:
    //     digitalWrite(PLLSR, HIGH);
    //     digitalWrite(PLLSM, HIGH);
    //     digitalWrite(PLLSL, LOW);
    //     break;
    // case 0x7:
    //     digitalWrite(PLLSL, HIGH);
    //     digitalWrite(PLLSR, HIGH);
    //     digitalWrite(PLLSM, HIGH);
    //     break;
    // default:
    //     digitalWrite(PLLSL, LOW);
    //     digitalWrite(PLLSM, LOW);
    //     digitalWrite(PLLSM, LOW);
    // }

    switch (LS)
    {
    case 1UL << 0:
        digitalWrite(PLLSL, HIGH);
    case 1UL << 1:
        digitalWrite(PLLSM, HIGH);
    case 1UL << 2:
        digitalWrite(PLLSR, HIGH);
    case ~(1UL << 0):
        digitalWrite(PLLSL, LOW);
    case ~(1UL << 1):
        digitalWrite(PLLSM, LOW);
    case ~(1UL << 2):
        digitalWrite(PLLSR, LOW);
    }

    switch(IR) {
        case 1UL << 0:
            digitalWrite(PLIRL, HIGH);
        case 1UL << 1:
            digitalWrite(PLIRR, HIGH);
        case ~(1UL << 0):
            digitalWrite(PLIRL, LOW);
        case ~(1UL << 1):
            digitalWrite(PLIRR, LOW);
    }

    if(USS <= 10) 
        digitalWrite(PLUSS, HIGH);
    else 
        digitalWrite(PLUSS, LOW);
}