/******************************************************************************
Tut-Tut_Piezo_Vibration_Sensor.ino
Jonathan Rohr, Michael Gillett, Shreyans Khunteta
February 24, 2019

- Connect a 1Mohm resistor across the Piezo sensor's pins.
- Connect one leg of the Piezo to GND
- Connect the other leg of the piezo to A0

Vibrations on the Piezo sensor create voltage, which are sensed by the Arduino's
A0 pin. Check the serial monitor to view the voltage generated.

Frequency, Intensity, and the Average drop impact voltage are all recorded and sent 
to a Google Sheet through the PushingBox API.
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

const int PIEZO_PIN = A0; // Piezo output

float duration = 120 ; // Specify the duration of time you want to record rainfall (in seconds)
int i = 0;
float average;
float frequency;
String intensity;
boolean loopReset = true;
unsigned long referenceTime = 0;
boolean raining = true;

// ================================================================ 
// ===                           SETUP                          ===
// ================================================================ 
void setup() 
{
  // LOOM_begin calls any relevant (based on config) LOOM device setup functions
  Loom_begin();
  digitalWrite(LED_BUILTIN, HIGH); // Show device is awake
  Serial.begin(9600);
}

// ================================================================ 
// ===                        MAIN LOOP                         ===
// ================================================================ 
void loop() 
{  
  OSCBundle send_bndl;
  measure_sensors();
  package_data(&send_bndl);

  gather_data(); // Gather the data from the sensor and record the results

  append_to_bundle_key_value(&send_bndl, "Average", average); // Add the average value to the bundle
  append_to_bundle_key_value(&send_bndl, "Frequency", frequency); // Add the frequency value to the bundle
  append_to_bundle_key_value(&send_bndl, "Intensity", intensity); // Add the intensity value to the bundle
  
  print_bundle(&send_bndl);
  log_bundle(&send_bndl, PUSHINGBOX); // Send the bundle to the Google Sheet using the PushingBox API
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
  // If it is not raining, go to sleep for X minutes
  if(raining == false){
    digitalWrite(LED_BUILTIN, LOW); // Show device is asleep
    sleep_for(2, MINUTES, SLEEPYDOG); // Go to sleep for X minutes
    digitalWrite(LED_BUILTIN, HIGH); // Show device is awake
    //delay(10000);

    // After coming out of sleep, check to see if it is raining
    int counter = 0;
    while(counter < 300){
      int piezoADC = analogRead(PIEZO_PIN);
      //3.3v / 4096bits * piezoADC = Piezo volts
      float piezoV = (3.3 / 4096) * piezoADC;
      if(piezoV > .26) {
        Serial.println(piezoV); // Print the voltage. 
        raining = true;
        break;
      }
      delay(100);
      counter++;
    }
  }
  // If it is raining, gather data
  if(raining == true){
    if(loopReset){
      referenceTime = millis()/1000;
    }
    do{
      // If timer is running, gather data and store data in array
      if(call_timer(0, duration) == true){
        // Read Piezo ADC value in, and convert it to a voltage
        int piezoADC = analogRead(PIEZO_PIN);
        //3.3v / 4096bits * piezoADC = Piezo volts
        float piezoV = (3.3 / 4096) * piezoADC;
        if(piezoV > .26) {
          Serial.println(piezoV); // Print the voltage. 
          sensorVals[i] = piezoV;
          i++;
          delay(150);
        }
        loopReset = false;
      }
      // If timer is done running, calculate the average, frequency, and intensity
      else if(call_timer(0, duration) == false){
        float temp = sensorVals[0];
        // Add all values from the sensorVals array
        if(i > 1){
          for(int j = 0; j < (i - 1); j++){
            temp = temp + sensorVals[j+1];
          }
        }
        
        // If there is no values in the array, then it is no longer raining
        if(i == 0){
          average = 0.0;
          frequency = 0.0;
          raining = false;
        }
        else{
          // Divide the added up values by the number of values in the array to calculate the average
          average = temp / i;
          // Divide the number of values in the array by the duration of the timer to calculate the frequency
          frequency = (i/duration);
        }

        float product = average * frequency;
        
        // Calculate the intensity by comparing the product of the average and frequency to set ranges of values
        if(average == 0){
          intensity = String("RainFree");
        }
        else if(0 < product && product <= .5){
          intensity = String("Light");
        }
        else if(.5 < product && product <= 1.0){
          intensity = String("Medium");
        }
        else if(1.0 < product){
          intensity = String("Heavy");
        }
        
        i = 0;
        sensorVals[0] = 0;
        loopReset = true;
      }
    }while(loopReset == false);
  }
}

// Timer function that runs for the specified duration
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
