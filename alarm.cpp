#include <Arduino.h>
#include <Wire.h>
#include <PCM.h>
#include "pt/pt.h"
#include "sound.h"

#define DEBOUNCE 10  // how many ms to debounce, 5+ ms is usually plenty
 

const int redButton =   7;     // the number of the pushbutton pin
const int greenButton = 6;     // the number of the pushbutton pin
const int blueButton =  5;

const int redLed =      4;      // the number of the LED pin
const int greenLed =    3;      // the number of the LED pin
const int blueLed =     2;

long randNumber;
int redState = 0;         // variable for reading the pushbutton status
int greenState = 0;  
int blueState = 0;

int difficulty = 0;
int alarm = 0;
int puzzle_array[12];
int solution_array[12];
int pointer = 0;

void array_print(int array_size);
void puzzle (int mod);

    int count2 = 0;

bool areEqual(int arr1[], int arr2[], int n);
void entered (int color);
void puzzle (int mod);
void good_morning();
void alarmFunc(int level);
int level = 0;
int set = 0;
int solved = 0;
      unsigned long lastTimeAlarm = 0;

static struct pt pt1;
static int protothreadBlinkLED1(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  PT_BEGIN(pt);
  while(1) {
    lastTimeBlink = millis();
    startPlayback(sample_data, sample_length);
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2100);
  }
  PT_END(pt);
}
void receiveEvent(int bytes) {
  level = Wire.read();    // read one character from the I2C
}

void setup() {
  startPlayback(sample_data, sample_length);
  PT_INIT(&pt1);
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  randomSeed(analogRead(0));
  // initialize the LED pin as an output:
  pinMode(redLed, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(redButton, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(greenButton, INPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(blueButton, INPUT);
  pinMode(13, OUTPUT);
  
}

void loop() {

  redState = digitalRead(redButton);
  greenState = digitalRead(greenButton);
  blueState = digitalRead(blueButton);
  solved = 0;

  if (level)  
    alarm = 1;
  if (alarm==1)
    protothreadBlinkLED1(&pt1);
  if (redState || greenState || blueState)
  {
    /* code */
    switch (level)
    {
    case 1:
      difficulty = 4;
      alarm=2;
      break;
    case 2:
      difficulty = 7;
      alarm=2;
      break;
    case 3:
      difficulty = 9;
      alarm=2;
      break;
      
    default:
      break;
    }

    if(alarm==2)
    {
      lastTimeAlarm = 0;
      lastTimeAlarm = millis();
      puzzle(difficulty);
    }
    if(alarm==3){

      do
      {
        if (millis() - lastTimeAlarm > 60000)
          break;
        
        redState = digitalRead(redButton);
        greenState = digitalRead(greenButton);
        blueState = digitalRead(blueButton);
        
        if (redState == HIGH) {
          digitalWrite(redLed, HIGH);
          entered(0);
          delay(500);
          digitalWrite(redLed, LOW);
        } else if (greenState == HIGH) {
          digitalWrite(greenLed, HIGH);
          entered(1);
          delay(500);
          digitalWrite(greenLed, LOW);
        } else if (blueState == HIGH) {
          digitalWrite(blueLed, HIGH);
          entered(2);
          delay(500);
          digitalWrite(blueLed, LOW);
        }
      } while(solved != 1);
    }
  }
}

void puzzle (int mod){
  
  for(int i=0; i<mod; i++){
    randNumber = random(3);
    puzzle_array[i]=randNumber;
    delay(100);
    switch (randNumber) {
        case 0:
          digitalWrite(redLed, HIGH);
          delay(500);
          digitalWrite(redLed, LOW);
          break;
        case 1:
          digitalWrite(greenLed, HIGH);
          delay(500);
          digitalWrite(greenLed, LOW);
          break;
        case 2:
          digitalWrite(blueLed, HIGH);
          delay(500);
          digitalWrite(blueLed, LOW);
          break;
    }
  }
  
  alarm = 3;
}
bool areEqual(int arr1[], int arr2[], int n) 
{ 
    // Linearly compare elements 
    for (int i = 0; i < n; i++) 
        if (arr1[i] != arr2[i]) 
            return false; 
  
    // If all elements were same. 
    return true; 
} 
void entered (int color){
  delay(250);
  solution_array[pointer]=color;
          digitalWrite(redLed, LOW);
          digitalWrite(greenLed, LOW);
          digitalWrite(blueLed, LOW);
  
  if(solution_array[pointer]==puzzle_array[pointer]){
    pointer++;
  }else{
    pointer=0;
  }
  if (pointer==(difficulty) && areEqual(solution_array, puzzle_array, difficulty)){
    good_morning();
    delay(60000);
    alarm = 0;
    level = 0;
    solved = 1;
  }
}
void good_morning(){
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
  delay(250);
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, HIGH);
  digitalWrite(blueLed, HIGH);
  delay(1000);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(blueLed, LOW);
}
