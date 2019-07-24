/* This sample code demonstrates the normal use of a TinyGPS object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
#include <SerialGSM.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
SerialGSM cell(6, 7);
TinyGPS gps;
SoftwareSerial ss(4, 5);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);
void send_sms()
{
  digitalWrite(3, HIGH);
  cell.SendSMS();
  Serial.println("SMS sent");
}
void setup()
{
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), send_sms, FALLING);
  Serial.begin(9600);
  cell.begin(9600);
  cell.Verbose(true);
  cell.Boot();
  cell.FwdSMS2Serial();
  cell.Rcpt("+91 **********");


  ss.begin(9600);
}
String str;

void loop()
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 8);
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 8);
  gps.stats(&chars, &sentences, &failed);
  Serial.println();
  //Serial.println(str);
  str = "I am in trouble. NEED HELP!!! http://www.google.com/maps/place/" + String(flat, 8) + "," + String(flon, 8);
  int l = str.length();
  char ch[l];
  for (int i = 0; i < l; i++)
  {
    ch[i] = str.charAt(i);
    Serial.print(ch[i]);
  }
  cell.Message(ch);
  smartdelay(1000);
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}
