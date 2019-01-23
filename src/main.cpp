#include <Arduino.h>

// pin declaration
#define PLIRL 2  // left IR-sensor
#define PLIRR 13 // Right IR-sensor
#define PLERR 8  // ERROR
#define PLLSR 7  // Right Light-sensor
#define PLLSL 3  // Left Light-sensor
#define PLLSM 4  // Middle Light-sensor
#define PLUSS 12 // Ultasoon-sensor
// light sensors
#define PLSR A0 // left       (LS << 0)
#define PLSM A1 // middle     (LS << 1)
#define PLSL A2 // right      (LS << 2)
// IR-transistors
#define PIRL A3 //  left    (IR << 0)
#define PIRR A4 //  right   (IR << 1)
//  HC-SR04
#define PECHO 6 // output signal
#define PTRIG 5 // input signal

#define PSL 10 // servo left
#define PSR 11 // servo right
// the setup function runs once when you press reset or power the board

char LS; // binary for the 2 lightsensors
char IR; // binary for the 3 infraredsensors
char LEDS;
unsigned long USS; // distance in cm for HC-SR04

// declare functions
void LightS(void);
void infraRedS(void);
void setMotors(signed char left, signed char right);
void distanceS(void);
void setLEDS(void);
bool laser;

void setup()
{
  //	set inputs light sensors
  pinMode(PLSR, INPUT);
  pinMode(PLSL, INPUT);
  pinMode(PLSM, INPUT);
  //	set inputs s
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

  pinMode(A5, INPUT);

  Serial.begin(9600);
}
// the loop function runs over and over again until power down or reset

int i = 0;

void loop()
{
  // set sensor variables
  if (!laser)
    infraRedS();
  LightS();
  distanceS();

  // if laser is seen SET variable laser. does not change when laser is not
  // visible anymore
  laser = (LS || laser) ? 1 : 0;
  // if LS or laser is greater than 0

  if (laser)
  {
    // if distance is longer than 7 cm
    if (USS >= 15)
    {
      switch (LS)
      {
      case 0x1:
        setMotors(0, 1);
        break;
        // if right and mid or only right see the laser
      case 0x4:
        setMotors(1, 0);
        break;
        // if mid or outer or all see the laser
      case 0x5:
      case 0x2:
      case 0x7:
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
      // stop. the job is done.
      setMotors(0, 0);
    }
  }

  else if (!laser)
  {
    if (USS >= 10)
    {
      switch (IR)
      {
      // if both are 1
      case 0x3:
        i = 0;
        if (USS <= 8)
          setMotors(1, 1);
        else
          setMotors(2, 2);

        break;
      // if right is 1
      case 0x2:
        i = 0;
        if (USS <= 8)
          setMotors(0, 1);
        else
          setMotors(0, 2);

        break;
      // if left is 1
      case 0x1:
        i = 0;
        if (USS <= 8)
          setMotors(1, 0);
        else
          setMotors(2, 0);

        break;
      // if none are 1
      case 0x0:
        i++;
        if (i < 800)
          setMotors(-1, 1);
        break;
      }
    }
    else
    {
      if (USS <= 5)
        setMotors(-2, -2);
      else
        setMotors(0, 0);
    }
  }
  setLEDS();
  Serial.println(USS);
}

void infraRedS(void)
{
  // read analog signals for IR sensors
  int L, R;
  L = pulseIn(PIRL, HIGH, 75000);
  R = pulseIn(PIRR, HIGH, 75000);

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

  if (L > 90)
    LS |= 1UL << 2;
  else
    LS &= ~(1 << 2);

  if (M > 80)
    LS |= 1UL << 1;
  else
    LS &= ~(1 << 1);

  if (R > 80)
    LS |= 1UL << 0;
  else
    LS &= ~(1 << 0);
}

void setMotors(signed char left = 0, signed char right = 0)
{

  switch (right)
  {
  case 2:
    analogWrite(PSR, 80);
    break;
  case 1:
    analogWrite(PSR, 120);
    break;
  case -1:
    analogWrite(PSR, 139);
    break;
  case -2:
    analogWrite(PSR, 200);
    break;
  default:
    analogWrite(PSR, 127);
    break;
  }
  switch (left)
  {
  case 2:
    analogWrite(PSL, 150);
    break;
  case 1:
    analogWrite(PSL, 137);
    break;
  case -1:
    analogWrite(PSL, 120);
    break;
  case -2:
    analogWrite(PSL, 80);
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
  unsigned long i = pulseInLong(PECHO, HIGH, 75000);
  USS = (i * 0.034 / 2 > 0 && i * 0.034 / 2 < 250) ? i * 0.034 / 2 : USS;
}

void setLEDS(void)
{

  digitalWrite(PLLSL, (LS & 0x1) ? HIGH : LOW);
  digitalWrite(PLLSM, (LS & 0x2) ? HIGH : LOW);
  digitalWrite(PLLSR, (LS & 0x4) ? HIGH : LOW);

  digitalWrite(PLIRL, (IR & 0x1) ? HIGH : LOW);
  digitalWrite(PLIRR, (IR & 0x2) ? HIGH : LOW);

  digitalWrite(PLUSS, ((laser && USS <= 7) || USS <= 10) ? HIGH : LOW);

  digitalWrite(PLERR, (((IR != 0x0 && laser) || (LS != 0x0 && !laser) ||
                        (USS > 1 && USS < 200))
                           ? LOW
                           : HIGH));
}