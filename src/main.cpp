#include <Arduino.h>

// the setup function runs once when you press reset or power the board

char LS; // hecadecimal for the 2 lightsensors
char IR; // hexadecimal for the 3 infraredsensors

unsigned long USS; // distance in cm for HC-SR04

// declare functions
void LightS(void);
void infraRedS(void);
void setMotors(char left, char right);
long unsigned distanceS(void);
bool laser;

byte PLSR = A0, // light sensor left
    PLSM = A1,  // light sensor middle
    PLSL = A1,  // light sensor right
    PIRL = A2,  // infrared sensor left
    PIRR = A3,  // infrared sensor right
    PECHO = 6,  // HC-SR04 output signal
    PTRIG = 5,  // HC-SR04 input signal
    PSL = 10,   // servo left
    PSR = 11;   // servo right

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

    Serial.begin(9600);
}
// the loop function runs over and over again until power down or reset
void loop()
{
    // set sensor variables

    LightS();
    infraRedS();
    distanceS();
    delay(200);

    // if laser is seen SET variable laser. does not change when laser is not visible anymore
    laser = (LS || laser) ? 1 : 0;
    // if LS or laser is greater than 0
    if (laser)
    {
        // if distance is longer than 5 cm
        if (USS >= 5)
        {
            switch (LS)
            {
                // if left and mid or only left see the laser
            case 0x1:
            case 0x3:
                if (USS <= 10)
                    setMotors(1, 0);
                else
                    setMotors(2, 1);
                break;

                // if right and mid or only right see the laser
            case 0x4:
            case 0x6:
                if (USS <= 10)
                    setMotors(0, 2);
                else
                    setMotors(1, 2);
                break;

                // if mid or outer or all see the laser
            case 0x5:
            case 0x2:
            case 0x7:
                if (USS <= 10)
                    setMotors(1, 1);
                else
                    setMotors(2, 2);
                break;

                // if laser disappeard
            case 0x0:

                break;
            }
        }
        // if distance is closer then 5cm
        else
        {
            // stop for a bit to let the leader get some distance
            setMotors(0, 0);
        }
    }

    else
    {
        if (USS >= 5)
        {
            switch (IR)
            {
            // if both are 1
            case 0x3:
                if (USS <= 5)
                    setMotors(1, 1);
                else
                    setMotors(2, 2);
                break;
            // if right is 1
            case 0x2:
                if (USS <= 5)
                    setMotors(1, 1);
                else
                    setMotors(2, 2);
                break;
            //if left is 1
            case 0x1:
                if (USS <= 5)
                    setMotors(1, 1);
                else
                    setMotors(2, 2);
                break;
            // if none are 1
            case 0x0:
                setMotors(-2, 2);
                break;
            }
        }
        else
        {
            setMotors(0, 0);
        }
    }
}

void infraRedS(void)
{
    // read analog signals for IR sensors
    int L, R;
    L = analogRead(PIRL);
    R = analogRead(PIRR);

    if (L > 10)
        // set 2th bit
        IR |= 1UL << 1;
    else
        // unset 2th bit
        IR &= ~(1 << 1);

    if (R > 10)
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

    if (L > 0)
        LS |= 1UL << 2;
    else
        LS &= ~(1 << 2);

    if (M > 0)
        LS |= 1UL << 1;
    else
        LS &= ~(1 << 1);

    if (R > 0)
        LS |= 1UL << 0;
    else
        LS &= ~(1 << 0);
}

void setMotors(char left = 0, char right = 0)
{
    int R = 0, L = 0;
    switch (right)
    {
    case 1:
    case 2:
        R = pow(right + 3, 4) / 6;
        break;
    case -1:
    case -2:
        R = pow(right - 3, 4) / 6;
        break;
    default:
        R = 0;
        break;
    }
    switch (left)
    {
    case 1:
    case 2:
        L = pow(left + 3, 4) / 6;
        break;
    case -1:
    case -2:
        L = pow(left - 3, 4) / 6;
        break;
    default:
        L = 0;
        break;
    }

    // right servo
    digitalWrite(PSR, HIGH);
    delayMicroseconds(1500 + R);
    digitalWrite(PSR, LOW);

    // left servo
    digitalWrite(PSL, HIGH);
    delayMicroseconds(1500 + L);
    digitalWrite(PSL, LOW);
    delay(2);
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
    unsigned long i = pulseIn(PECHO, HIGH, 30);
    USS = i * 0.034 / 2;
    return USS;
}