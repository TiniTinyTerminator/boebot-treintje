#include <Arduino.h>

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

byte PLIRL = 2, // left IR-sensor
    PLIRR = 13, // Right IR-sensor
    PLERR = 8,  // ERROR
    PLLSR = 7,  // Right Light-sensor
    PLLSL = 3,  // Left Light-sensor
    PLLSM = 4,  // Middle Light-sensor
    PLUSS = 12, // Ultasoon-sensor
    // light sensors
    PLSR = A0, // left       (LS << 0)
    PLSM = A1, // middle     (LS << 1)
    PLSL = A2, // right      (LS << 2)
    // IR-transistors
    PIRL = A3, //  left    (IR << 0)
    PIRR = A4, //  right   (IR << 1)
    //  HC-SR04
    PECHO = 6, // output signal
    PTRIG = 5, // input signal

    PSL = 10, // servo left
    PSR = 11; // servo right

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

  // Serial.print(LS, BIN);
  // Serial.print("\n");
  // Serial.print(USS, DEC);
  // Serial.print("\n");
  // Serial.print(IR, BIN);
  // Serial.print("\n\n");

  // if laser is seen SET variable laser. does not change when laser is not
  // visible anymore
  laser = (LS || laser) ? 1 : 0;
  // if LS or laser is greater than 0

  if (laser)
  {
    // if distance is longer than 7 cm
    if (USS >= 7)
    {
      switch (LS)
      {
        // if left and mid or only left see the laser
      case 0x3:
      case 0x1:
        if (USS <= 15)
          setMotors(0, 1);
        else
          setMotors(0, 2);
        break;
        // if right and mid or only right see the laser
      case 0x6:
      case 0x4:
        if (USS <= 15)
          setMotors(1, 0);
        else
          setMotors(2, 0);

        break;
        // if mid or outer or all see the laser
      case 0x5:
      case 0x2:
      case 0x7:
        if (USS <= 15)
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
      // stop. the job is done.
      setMotors(0, 0);
    }
  }

  else
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
        if (i < 1000)
          setMotors(-2, 2);

        break;
      }
    }
    else
    {
      setMotors(-2, -2);
    }
  }
  setLEDS();

  // Serial.print(analogRead(A5));
  // Serial.print("\n");
  // Serial.print(digitalRead(A5));
  // Serial.print("\n");
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

  if (L > 110)
    LS |= 1UL << 2;
  else
    LS &= ~(1 << 2);

  if (M > 110)
    LS |= 1UL << 1;
  else
    LS &= ~(1 << 1);

  if (R > 110)
    LS |= 1UL << 0;
  else
    LS &= ~(1 << 0);
}

void setMotors(signed char left = 0, signed char right = 0)
{

  switch (right)
  {
  case 2:
    analogWrite(PSR, 100);
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
    analogWrite(PSL, 100);
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
  unsigned long i = pulseInLong(PECHO, HIGH, 50000);
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

  digitalWrite(PLERR, (((IR != 0x0 && laser) || (LS != 0x0 && laser) || (USS > 0 && USS < 240)) ? LOW : HIGH));
}