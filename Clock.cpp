
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <DS1302/DS1302.h>
#include <pt/pt.h>
#include <Wire.h>
#include "Rotary.h"

#define SELECTBUTTON 8


Rotary rotary = Rotary(12, 13);
DS1302 rtc(5,6,7);
LiquidCrystal_I2C lcd(0x27,20,4);
// Init a Time-data structure
Time t;

uint8_t hr = 0;
uint8_t min = 0;
uint8_t alarmHr = 0;
uint8_t alarmMin = 0;
uint8_t sec = 0;
uint8_t flag = 0;
int level = 0;
uint8_t *val;

void print2digits(uint8_t number);
void printVoid();
void adjustClock();

boolean buttonActive = false;
boolean longPressActive = false;

void myClock();
void adjustAlarm();
void printAlarm();
void adjustDifficulty();
void alarm();
void limit();

void setup()
{
  pinMode(SELECTBUTTON, INPUT_PULLUP);
  Wire.begin(); 

  rtc.halt(false);
  rtc.writeProtect(false);
  
  lcd.begin(16,2);
  lcd.backlight();
  lcd.clear();
  lcd.print("00:00:00");
  adjustClock();
}

void loop()
{
myClock();
}
void helloMessage();
void adjustClock() {
  while (flag!=2)
  {
    switch (flag)
    {
    case 0:
      val=&hr;
      lcd.setCursor(0,0);
      break;
    case 1:
      val=&min;
      lcd.setCursor(3,0);
      break;
    }
      /* code */
      unsigned char result = rotary.process();
      if (result == DIR_CW) {
        (*val)++;
      } else if (result == DIR_CCW) {
        (*val)--;
      }
      limit();
      
      print2digits(*val);
    
    if (digitalRead(SELECTBUTTON) == LOW )  {
      delay(500);
      ++flag;
    }
  }
  flag = 0;
  rtc.setTime(hr, min, 00);
  rtc.setDOW(FRIDAY);
  rtc.setDate(6, 8, 2010);

  lcd.setCursor(0,1);
  lcd.print("00:00");
}

void myClock()  {
  t = rtc.getTime();
  
  if (sec != t.sec)
  {
    sec=t.sec;
    lcd.setCursor(0,0);
    print2digits(t.hour);
    lcd.print(':');
    print2digits(t.min);
    lcd.print(':');
    print2digits(t.sec);
  }

  switch (flag)
  {
  case 0:
    val=&alarmHr;
    adjustAlarm();
    break;
  case 1:
    val=&alarmMin;
    adjustAlarm();
    break;
  case 2:
    adjustDifficulty();
    break;
  case 3:
    alarm();
  default:
    break;
  }
  if (digitalRead(SELECTBUTTON) == LOW )  {
    delay(500);
    ++flag;
    if (flag == 4)
      flag=0;
  }   
}

void adjustAlarm() {
  unsigned char result = rotary.process();
  if (flag==0)
  {
    lcd.setCursor(0,1);
  } else if (flag == 1)
  {
    lcd.setCursor(3,1);
  }
  
  if (result == DIR_CW) {
    (*val)++;
  } else if (result == DIR_CCW) {
    (*val)--;
  }
  limit();
  print2digits(*val);

}

void adjustDifficulty() {
  unsigned char result = rotary.process();
    if (result == DIR_CW) {
    level++;
  } else if (result == DIR_CCW) {
    level--;
  }
  lcd.setCursor(5,1);
  switch (level)
  {
  case 1:
    lcd.print(" *  ");
    break;
  case 2:
    lcd.print(" ** ");
    break;
  case 3:
    lcd.print(" ***");
    break;
  case 4:
    level = 3;
    break;
  default:
    lcd.print("    ");
    level = 0;
    break;
  }
}

void alarm(){
  if (level >= 0 && alarmHr == t.hour && alarmMin == t.min)
  {
    Wire.beginTransmission(9); // transmit to device #9 
    Wire.write(level);         // sends level
    Wire.endTransmission();    // stop transmitting
  }
  
}

void print2digits(uint8_t number) {
  // Output leading zero
  if (number >= 0 && number < 10) {
    lcd.print('0');
  }
  lcd.print(number);
}

void limit(){
  Serial.println("limit");
  if (flag==0)
    {
      if ((*val)==24)
      {
        (*val)=0;
      } else if ((*val)==255)
      {
        (*val)=23;
      }
    } else if (flag==1)
    {
      if ((*val)==60)
      {
        (*val)=0;
      } else if ((*val)==255)
      {
        (*val)=59;
      }
    }
}
