/* 
Code for distance measurement and Alert system using UWB technology and RYUW122 modules by www.playwithcircuit.com
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans12pt7b.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>

#define BUZZER_PIN 2

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Format -> (Address,Width,Height )

//Create software serial object to communicate with UWB Module
SoftwareSerial UWB_Serial(9, 10);  // Now pin 9 and pin 10 of Arduino are Serial Rx & Tx pin respectively.

char serialInputArray[50];
int counter = 0;  // Counter for getting data from UWB module

const char cmd_array[] = "AT+ANCHOR_SEND=TAG,2,Hi";

bool boAlarm_flag = false;

unsigned int u32previoustick;
unsigned int u32previoustickBuzzer;
int buzzerpinvalue = 1;
// SETUP
void setup() {
  int retVal = 0;

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN,buzzerpinvalue);
  // this serial port is to display error, if occurred during OLED init
  Serial.begin(9600);

  // initialize the lcd
  lcd.init();
  // Turn on the Backlight
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance");
  lcd.setCursor(0, 1);
  lcd.print("Measuement");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Using REYAX");
  lcd.setCursor(0, 1);
  lcd.print("RYUW122 Modules");
  delay(2000);

  // Open the soft Serial port and set the data rate for the SoftwareSerial port at 115200 to communicate with UWB Module in Command Mode
  UWB_Serial.begin(9600);
  delay(100);
  // Send data to UWB Module
  UWB_Serial.print("AT");
  printCRLF_UWB();

  retVal = vCheck_OK();
  if (retVal == -1) {
    errorMsg();
    errorMsgLCD();
    while (1)
      ;
  } else {
    successMsg();
    successMsgLCD();
  }

  u32previoustick = millis();
  u32previoustickBuzzer = millis();
}

void loop() {
  int retVal = 0;
  int Distance_in_Cm = 0;

  // Send data to UWB Module
  if((millis() - u32previoustick) > 1000)
  {
    u32previoustick = millis();
    flushUWBRcv();
    Serial.print("\n\r\n\rSend Distance cmd");
    UWB_Serial.print(cmd_array);
    printCRLF_UWB();

    delay(50);
    // Now check for distance
    retVal = vCheck_Distance(&Distance_in_Cm);
    if (retVal == 0) {
      Serial.print("\n\rDistance = ");
      Serial.print(Distance_in_Cm);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Distance = ");
      lcd.setCursor(0, 1);
      lcd.print(Distance_in_Cm);
      lcd.print(" cm");
      if(Distance_in_Cm > 200 )
      {
        boAlarm_flag = true;
      }
    } else {
      errorMsg();
    }
  }

  if(boAlarm_flag == true)
  {
    if((millis() - u32previoustickBuzzer) > 500)
    {
      u32previoustickBuzzer = millis(); 
      buzzerpinvalue = !buzzerpinvalue;
      digitalWrite(BUZZER_PIN,buzzerpinvalue);
    }
  }  
}

// Error MSG when Communication with UWB Module is not possible
void errorMsg() {
  Serial.print("\n\rUWB Module Comm Fail");
}

void errorMsgLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UWB Module");
  lcd.setCursor(0, 1);
  lcd.print("Comm Failed");
}

// Error MSG when Communication with UWB Module is not possible
void successMsg() {
  Serial.print("\n\rUWB Module Reply Comm Success");
}

void successMsgLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UWB Module");
  lcd.setCursor(0, 1);
  lcd.print("Comm Success");
}

// This function sends "\n\r" after Every Command to UWB
void printCRLF_UWB() {
  UWB_Serial.write(13);  // Carriage Return '\r'
  UWB_Serial.write(10);  // Line Feed       '\n'
}

// This function sends "\n\r" after Every Command to Serial Terminal
void printCRLF() {
  Serial.write(13);  // Carriage Return '\r'
  Serial.write(10);  // Line Feed       '\n'
}

// Flush Extra character from UWB Rx buffer
void flushUWBRcv() {
  char ret_char;
  while (UWB_Serial.available() > 0) {
    ret_char = UWB_Serial.read();
  }
}

int vCheck_OK() {
  // Check if data is available on UWB Serial Port
  do {
    delay(10);
    // If counter is greater than 300 i.e., 3000 ms has passed then break out of Loop
    if (counter++ > 300) {
      break;
    }
  } while (UWB_Serial.available() == 0);

  // If counter is greater than 300 i.e., 3000 ms has passed and data is not received
  if (counter > 300) {
    Serial.print("\n\rNo character Rcvd in OK");
    return -1;
  } else {
    counter = 0;
    do {
      serialInputArray[counter++] = UWB_Serial.read();
      delay(10);  // delay for 10 ms for getting response if the Module is slow
    } while (UWB_Serial.available() != 0);
    if (memcmp(serialInputArray, "+OK", 3) == 0) {
      return 0;
    } else {
      Serial.print(serialInputArray);
      return -1;
    }
  }
}

int vCheck_Distance(int *pDistance_in_Cm) {
  int comma_counter = 0;
  int distance = 0;
  int distance_found = 0;

  // Wait for data
  counter = 0;

  do {
    delay(10);
    if (counter++ > 300) {
      Serial.print("\n\rNo character Rcvd in distance");
      return -1;
    }

  } while (UWB_Serial.available() == 0);

  // Read received data
  int rxIndex = 0;

  while (UWB_Serial.available() && rxIndex < 49) {
    serialInputArray[rxIndex++] = UWB_Serial.read();
    delay(10);
  }

  serialInputArray[rxIndex] = '\0';


  Serial.print("\n\rReceived Reply ");
  Serial.print(serialInputArray);

  // Find third comma
  for (int i = 0; i < rxIndex; i++) {
    if (serialInputArray[i] == ',') {
      comma_counter++;

      if (comma_counter == 3) {
        // Distance starts immediately after third comma
        for (int j = i + 1; j < rxIndex; j++) {
          if (serialInputArray[j] >= '0' && serialInputArray[j] <= '9') {
            distance = distance * 10 + (serialInputArray[j] - '0');

            distance_found = 1;
          } else if (distance_found) {
            // First non-digit after distance
            break;
          }
        }

        if (distance_found) {
          *pDistance_in_Cm = distance;
          return 0;
        }

        return -1;
      }
    }
  }

  return -1;
}
