#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
String COMMANDSTATE = "ON";
#define PIN 7 //This is the Digital Pin the RGB LEDs will be connected too
#define STARTPIXELS 0
#define NUMPIXELS 65
#define ENDPIXELS 150
//LED STRIP
Adafruit_NeoPixel LEDS = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);

//EEPROM FUNCTIONS
void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
{
  byte lowByte = ((p_value >> 0) & 0xFF);
  byte highByte = ((p_value >> 8) & 0xFF);

  EEPROM.write(p_address, lowByte);
  EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
  byte lowByte = EEPROM.read(p_address);
  byte highByte = EEPROM.read(p_address + 1);

  return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

//Globals
uint32_t CURRENT_COLOR = EEPROMReadlong(1);
int CURRENT_BRIGHTNESS = EEPROMReadInt(20);
//-----------------------------------------

void setup()
{
  pinMode(PIN, OUTPUT);
  Serial.begin(9600);
  LEDS.begin();
  LEDS.setBrightness(CURRENT_BRIGHTNESS);
  for(int i = 0; i < NUMPIXELS; i++)
  {
    LEDS.setPixelColor(i, CURRENT_COLOR);

  }
  LEDS.show();
}

void loop()
{
  if (Serial.available() > 0)
  {
    inputToOutput();
  }
}

void inputToOutput()
{
  String command = Serial.readStringUntil('\n');

  if (command == "ON")
  {
    ON();
  }
  else if (command == "OFF")
  {
    OFF();
  }
  else if (command == "SOLID")
  {
    SOLID(Serial.readStringUntil('\n'));
  }
  else if (command == "BRIGHTER")
  {
    BRIGHTER();
  }
  else if (command == "DARKER")
  {
    DARKER();
  }
}

void ON()
{
  if (LEDS.getBrightness() == 0)
  {
    LEDS.setBrightness(EEPROMReadInt(20));
    for (int i = 0; i < NUMPIXELS; i++)
    {
      LEDS.setPixelColor(i, CURRENT_COLOR);
    }
      LEDS.show();
  }
}

void OFF()
{
  LEDS.setBrightness(0);
//  CURRENT_BRIGHTNESS = LEDS.getBrightness();
//  EEPROMWriteInt(20, CURRENT_BRIGHTNESS);
  LEDS.show();
}

void SOLID(String color)
{
  uint32_t colorVar = parseColorString(color);
  writeColorToEEPROM(color);
  CURRENT_COLOR = colorVar;
  for (int i = 0; i < NUMPIXELS; i++)
  {
    LEDS.setPixelColor(i, colorVar);
  }
  LEDS.show();
}

void BRIGHTER()
{
  if (LEDS.getBrightness() + 10 < 255)
  {
    LEDS.setBrightness(LEDS.getBrightness() + 10);
    CURRENT_BRIGHTNESS = LEDS.getBrightness();
    EEPROMWriteInt(20, CURRENT_BRIGHTNESS);
  }
  LEDS.show();
}

void DARKER()
{
  if (LEDS.getBrightness() - 10 > 0)
  {
    LEDS.setBrightness(LEDS.getBrightness() - 10);
    CURRENT_BRIGHTNESS = LEDS.getBrightness();
    EEPROMWriteInt(20, CURRENT_BRIGHTNESS);
  }
  LEDS.show();
}

void writeColorToEEPROM(String color)
{
  EEPROMWritelong(1, parseColorString(color));
}

uint32_t parseColorString(String color)
{
  char str[100];
  color.toCharArray(str,100);
  float Red = atoi(strtok(str," "));
  float Green = atoi(strtok(NULL," "));
  float Blue = atoi(strtok(NULL," "));
  return LEDS.Color(Red,Green,Blue);
}
