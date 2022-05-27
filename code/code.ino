#include <Keypad.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(4, 3, 2, A1, A2, A3);

int values[5] = {0, 0, 0, 0, 0};
char code [4] = {'1', '2', '3', '4'};
int readings = 0;
bool codeIsRight = true;
int buzzer = 5;

const byte numRows= 4;   
const byte numCols= 4;
 
char keymap[numRows][numCols]= 
          {
          {'1', '2', '3', 'A'}, 
          {'4', '5', '6', 'B'}, 
          {'7', '8', '9', 'C'},
          {'*', '0', '#', 'D'}
          };

byte rowPins[numRows] = {13,12,11,10};
byte colPins[numCols]= {9,8,7,6}; 

Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Alarm active.");  
  delay(2000);
}

void shiftElements(int arr[]) {
  for(int i = 1; i < 5; i++) {
    arr[i - 1] = arr[i];
  }  
}

float average(int arr[]) {
  float sum = 0;
  for(int i = 0; i < 5; i++) {
      sum += arr[i];
  }    
  return sum / 5.0f;  
}

void alarm() {
  while(true) {
    tone(buzzer, 3000);
    delay(100);
    noTone(buzzer);
    delay(100);
  }  
}

bool insertCode() {
  tone(buzzer, 1000);
  
  int n = 0;
  for(int i = 0; i < 4; i++) {
    char keypressed = myKeypad.waitForKey();
    if (keypressed==code[i]) {
      n++;
    }    
    lcd.print("*");
    Serial.println(n);
  }
  if(n == 4) {
    codeIsRight = true;
    return true;
  }
  codeIsRight = false;
  return false;
  lcd.print("");
}

void loop() {
  int value = analogRead(A0);

  if(readings < 5){
    readings++;
  }
  
  shiftElements(values);
  values[4] = value;
  delay(10);

  if(abs(value - average(values)) > 50 && readings == 5) {
    lcd.clear();
    lcd.print("Intruder detected.");
    lcd.setCursor(0,1);
    lcd.print("Password: ");
    for(int i = 0; i < 3; i++){
      if(insertCode()) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Alarm active.");
        noTone(buzzer);
        break;
      }
      else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tries left: ");
        lcd.print(2 - i);
        lcd.setCursor(0,1);
        lcd.print("Password: ");
      }
    }
    if(!codeIsRight) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Emergency service");
      lcd.setCursor(0,1);
      lcd.print("has been called.");
      alarm();
    }
    readings = 0;
  }

  Serial.println(myKeypad.getKey());
}
