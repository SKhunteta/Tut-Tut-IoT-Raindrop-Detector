/******************************************************************************
Tut-Tut_Piezo_Vibration_Sensor.ino
Jonathan Rohr
February 24, 2019

- Connect a 1Mohm resistor across the Piezo sensor's pins.
- Connect one leg of the Piezo to GND
- Connect the other leg of the piezo to A0

Vibrations on the Piezo sensor create voltage, which are sensed by the Arduino's
A0 pin. Check the serial monitor to view the voltage generated.
******************************************************************************/

// ================================================================
// ===              INCLUDE CONFIGURATION FILE                  ===
// ===    INCLUDE DECLARATIONS, STRUCTS, AND FUNCTIONS FROM     ===
// ===            OTHER FILES AS SET IN CONFIG.H                ===
// ================================================================
// Config has to be first has it hold all user specified options
#include "config.h"

// Preamble includes any relevant subroutine files based 
// on options specified in the above config
#include "loom_preamble.h"

#include "arduino_secrets.h"
#include <Adafruit_SleepyDog.h>

const int PIEZO_PIN = A0; // Piezo output
const int ledPIN = 13; // LED connected to digital pin 13

//float sensorVals[4000];     // create sensor values storage array
//float sensorAvgs[100];
//float sensorNumDrops[100];
//float sensorFreq[100];
float duration = 5;
int i = 0;
int avg = 0;
int n = 0;
//int f = 0;
int t = 0;
float average;
float frequency;
String intensity;
boolean loopReset = true;
unsigned long referenceTime = 0;
int ledState = LOW; // variable used to store the last LED status, to toggle the light
boolean raining = true;

// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{
  // LOOM_begin calls any relevant (based on config) LOOM device setup functions
  Loom_begin(); 
  //WiFi.setPins(8,7,4,2);
  pinMode(ledPIN, OUTPUT); // declare the ledPin as the OUTPUT
  digitalWrite(ledPIN, HIGH); // Show device is awake
  Serial.begin(9600);
  //attachInterrupt(PIEZO_PIN, wakeUpNow, LOW);
}

// ================================================================ 
// ===                        MAIN LOOP                         ===
// ================================================================ 
void loop() 
{  
  OSCBundle send_bndl;
  measure_sensors();
  package_data(&send_bndl);

  gather_data();

  append_to_bundle_key_value(&send_bndl, "Average", average);
  append_to_bundle_key_value(&send_bndl, "Frequency", frequency);
  append_to_bundle_key_value(&send_bndl, "Intensity", intensity);
  
  print_bundle(&send_bndl);
  log_bundle(&send_bndl, PUSHINGBOX);
  additional_loop_checks();
}

//void loop() 
//{
//  OSCBundle send_bndl;      // Declare bundles to hold incoming and outgoing data
//
//  measure_sensors();        // Read sensors, store data in sensor state struct
//  package_data(&send_bndl);   // Copy sensor data from state to provided bundle
//  
//  int analog0 = state_analog.a0;      // directly access analog state struct
//  int analog4 = read_analog(4);   // read analog 4 as you would if not 
//                                        // integrated into Loom (should just enable 
//                                        // A4 in the config though)
//  float f     = 4.56;
//  char* c_str = "test";
//  String str  = String("test2");
//
//  append_to_bundle_key_value(&send_bndl, "analog0", analog0);
//  append_to_bundle_key_value(&send_bndl, "analog4", analog4);
//  append_to_bundle_key_value(&send_bndl, "float", f);
//  append_to_bundle_key_value(&send_bndl, "c-str", c_str);
//  append_to_bundle_key_value(&send_bndl, "String", str);
//
//  print_bundle(&send_bndl);
//  
//  log_bundle(&send_bndl, PUSHINGBOX);
//  
//  additional_loop_checks();     // Miscellaneous checks
//}

//void loop() 
//{
//  OSCBundle bndl;
//
//  // --- Basic Example ---
//
//  // receive_bundle(&bndl, WIFI);
//  // process_bundle(&bndl);
//
//  measure_sensors();        // Read sensors, store data in sensor state struct
//  package_data(&bndl);      // Copy sensor data from state to provided bundle
//
//  append_to_bundle_key_value(&bndl, "pin5", digitalRead(5));
//  append_to_bundle_key_value(&bndl, "pin6", digitalRead(6));
//  // append_to_bundle_key_value(&bndl, "pin7", digitalRead(7));
//  // append_to_bundle_key_value(&bndl, "pin8", digitalRead(8)); 
//  // append_to_bundle_key_value(&bndl, "pin9", digitalRead(9));
//  // append_to_bundle_key_value(&bndl, "pin10", digitalRead(10));
//
//  // append_to_bundle_key_value(&bndl, "pin10", digitalRead(10));
//
//
//  print_bundle(&bndl);
//  
//
//  // log_bundle(&bndl, SDCARD, "evapdata.csv");
//  // send_bundle(&bndl, WIFI);
//  // 
//  delay(100);
//
//  additional_loop_checks();   // Miscellaneous checks
//
//  // --- End Example ---
//
//
//
//  // // --- OLED Example ---
//
//  // measure_sensors();     // Read sensors, store data in sensor state struct
//  // package_data(&bndl);     // Copy sensor data from state to provided bundle
//  
//  // print_bundle(&bndl);
//  // log_bundle(&bndl, OLED);
//
//  // delay(1000);
//
//  // additional_loop_checks();  // Miscellaneous checks
//
//  // // --- End Example ---
//
//
//
//  // // --- LoRa Hub Example ---
//
//  // receive_bundle(&bndl, LORA);   // Read sensors, store data in sensor state struct
//  // print_bundle(&bndl);
//  // log_bundle(&bndl, PUSHINGBOX);
//
//  // additional_loop_checks();    // Miscellaneous checks
//  // // --- End Example ---
//
//
//
//  // // --- LoRa Node Example ---
//
//  // measure_sensors();     // Read sensors, store data in sensor state struct
//  // package_data(&bndl);     // Copy sensor data from state to provided bundle
//  
//  // print_bundle(&bndl);
//
//  // log_bundle(&bndl, SDCARD, "savefile.csv");
//  // send_bundle(&bndl, LORA);
//
//  // delay(1000);
//
//  // additional_loop_checks();  // Miscellaneous checks
//  // // --- End Example ---
//
//
//
//  // // --- LoRa Evaporometer Example ---
//
//  // measure_sensors();     // Read sensors, store data in sensor state struct
//  // package_data(&bndl);     // Copy sensor data from state to provided bundle
//  
//  // print_bundle(&bndl);
//
//  // log_bundle(&bndl, SDCARD, "savefile.csv");
//  // send_bundle(&bndl, LORA);
//
//  // sleep_for(5, MINUTES, STANDBY);
//
//  // additional_loop_checks();  // Miscellaneous checks
//  // // --- End Example ---
//
//
//
//  // // --- Common Example ---
//
//  // receive_bundle(&bndl, WIFI); // Receive messages
//  // if (bndl.size()) {
//  //  print_bundle(&bndl);    // Print bundle if LOOM_DEBUG enabled
//  // }
//  // process_bundle(&bndl);     // Dispatch message to correct handling functions
//
//  // measure_sensors();       // Read sensors, store data in sensor state struct
//  // package_data(&bndl);     // Copy sensor data from state to provided bundle
//
//  // // print_bundle(&bndl);      // Print bundle if LOOM_DEBUG enabled
//
//  // send_bundle(&bndl, WIFI);    // Send bundle of packaged data
//  // // log_bundle(&bndl, PUSHINGBOX);  // Send bundle to Google Sheet
//  // // log_bundle(&bndl, SDCARD, "Ishield.csv"); // Send bundle to Google Sheet
//
//  // additional_loop_checks();      // Miscellaneous checks
//  // // --- End Example ---
//
//
//
//
//}


// ================================================================ 
// ===                 High-Level API Functions                 === 
// ================================================================ 

// void receive_bundle(OSCBundle *bndl, CommPlatform platform);
// void process_bundle(OSCBundle *bndl);
// void measure_sensors();
// void package_data(OSCBundle *bndl);
// void send_bundle(OSCBundle *bndl, CommPlatform platform, int port);
// void send_bundle(OSCBundle *bndl, CommPlatform platform);
// void log_bundle(OSCBundle *bndl, LogPlatform platform, char* file); // filename required for SD files
// void log_bundle(OSCBundle *bndl, LogPlatform platform);
// bool bundle_empty(OSCBundle *bndl);
// void additional_loop_checks();
// void sleep_for(int amount, TimeUnits units, SleepMode mode);
// void sleep_until_time(SleepMode mode, int hour, int min, int sec);
// void append_to_bundle_key_value(OSCBundle *bndl, char* key, T elem);

// CommPlatforms: WIFI, LORA, NRF
// LogPlatforms:  PUSHINGBOX, SDCARD, OLED
// TimeUnits: MINUTES, SECONDS
// SleepMode: STANDBY, SLEEPYDOG

// Print Macro
// LOOM_DEBUG_Println

void gather_data(){
  float sensorVals[4000];
  if(raining == false){
    digitalWrite(ledPIN, LOW); // Show device is asleep
    attachInterrupt(PIEZO_PIN, wakeUpNow, RISING);
    int sleepMS = Watchdog.sleep();
    detachInterrupt(A0);
  }
  else if(raining == true){
    if(loopReset){
      referenceTime = millis()/1000;
    }
    do{
      if(call_timer(0, duration) == true){
        // Read Piezo ADC value in, and convert it to a voltage
        int piezoADC = analogRead(PIEZO_PIN);
        float piezoV = piezoADC / 1023.0 * 5.0;
        if(piezoV > .1) {
          Serial.println(piezoV); // Print the voltage.
          sensorVals[i] = piezoV;
          i++;
          delay(100);
        }
        loopReset = false;
      }
      else if(call_timer(0, duration) == false){
        float temp = sensorVals[0];
        for(int j = 0; j < i; j++){
          temp = temp + sensorVals[j+1];
        }
        average = temp / (i + 1);
        frequency = ((i+1)/duration);
        switch(average){
          case (0 < average && average <= 35): intensity = String("Light");
          case (35 < average && average <= 50): intensity = String("Medium");
          case (50 < average && average <= 100): intensity = String("Heavy");
        }
        //sensorAvgs[avg] = average;
        //sensorNumDrops[n] = (i+1);
        //sensorFreq[f] = ((i+1)/duration);
        //f++;
        //n++;
        //avg++;
        i = 0;
        loopReset = true;
      }
    }while(loopReset == false);
//    if(avg == 10){
//      for(int a = 0; a < 10; a++){
//        Serial.println(sensorAvgs[a]);
//      }
//      Serial.println("----------------------");
//      for(int b = 0; b < 10; b++){
//        Serial.println(sensorNumDrops[b]);
//      }
//      Serial.println("----------------------");
//      for(int c = 0; c < 10; c++){
//        Serial.println(sensorFreq[c]);
//      }
//      avg = 0;
//    }
  }
}


boolean call_timer(int start, int duration){
  unsigned long timer = millis()/1000;
  unsigned long startTime = referenceTime + start;
  unsigned long endTime = startTime + duration;
  if(timer >= startTime && timer <= endTime){
    return true;
  }
  else{
    return false;
  }
}

void wakeUpNow(){
  digitalWrite(ledPIN, HIGH); // Show device is awake
  raining = true;
}
