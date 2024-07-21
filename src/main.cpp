// Lbraries
#include <Arduino.h>        // Basic Library
#include <math.h>           // Math Library
#include <Wire.h>           // I2C Library
#include <U8g2lib.h>        // Display Library
#include <MUIU8g2.h>        // Menu LIbrary
#include <BME280I2C.h>      // Temp/Hum/Pres sensor Library
#include <ADS1X15.h>        // ADC / Amplifier Library
#include <RunningAverage.h> // Running Average Library
#include "EEPROM.h"

// Custom Headers
#include "constants.h"   // Global Constants
#include "oxygen.h"      // oxygen calculations
#include "environment.h" // environmenttal parameters: temperature, humidity, atmospheric pressure
#include "helium.h"      // ultrasonic measurement and helium calculations
#include "menu.h"       // menu parameters and definitions, used to clean up top of this file

void setup(void)
{
        // start serial connection
        Serial.begin(115200);
        delay(1000); // Delay to stabilize serial communication

        if (!EEPROM.begin(eeprom_size)) {
                Serial.println("Failed to initialize EEPROM");
                Serial.println("Restarting...");
                delay(1000);
                ESP.restart();
        }  

        // Initialise I2C
        Wire.begin(sda_pin, scl_pin);


        // buttons and display Initialise
        if (menu_initialise()) {
                Serial.println("Display Connected");
                splash_screen();
                delay(500);
        }

        // Temperature, Relative Humidity, and pressure Sensor Initialise
        if (!Environment_Initialise()) {
                Serial.println("Environment not connected");
                delay(500);
        }

        // Oxygen sensor Initialise
        if (O2_Initialise()) {
                Serial.println("ADC Connected");
                delay(500);
        }

        // Oxygen sensor Initialise
        if (He_Initialise()) {
                Serial.println("Ultrasonic Connected");
                delay(500);
        }

        load_calibration_values();

        delay(1500);

        run_menu();
        
        Serial.println("Ready");
}

void loop(void)
{
        check_button_event();

        if (mui_active())
                submenu_draw();
        else 
                submenu_cases();
}
