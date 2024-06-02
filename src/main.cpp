// Lbraries
#include <Arduino.h>          // Basic Library
#include <math.h>             // Math Library
#include <Wire.h>             // I2C Library
#include <Adafruit_HTU21DF.h> // Temp/Hum sensor Library
#include <Adafruit_GFX.h>     // Graphic Library
#include <Adafruit_SSD1306.h> // Display Library
#include <Adafruit_ADS1X15.h> // ADC / Amplifier Library
#include <RunningAverage.h>   // Running Average Library

// Custom Headers
#include "constants.h"      // Global Constants
#include "oxygen.h"         // oxygen calculations
#include "temp_hum.h"       // temperature, humidity, water% calcs
#include "ultrasonic.h"     // ultrasonic measurement
#include "speed_of_sound.h" // speed of sound calculations
#include "menu.h"           // menu logic
#include "helium.h"      // helium calculations

// function declarations

void displayValues(double, double, double, double);
void serialdisplayValues(double, double, double, double, double, double);

void Display_Initialise();

// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Oxygen Analysis setup
double O2_calibration; // Calibration value (%/mV)

// calibrated distance in m
double dist_cal = 0.04805;



void setup()
{
    // start serial connection
    Serial.begin(115200);
    delay(1000); // Delay to stabilize serial communication

    // Initialise I2C
    Wire.begin();

    // Temp Sensor Initialise
    Temp_Initialise();

    // Display Initialise
    Display_Initialise();

    // O2 Initialise
    O2_Initialise();

    // Ultrasonic initialise
    ultrasonic_Initialise();

    // helium initialization
    He_Initialise();

    O2_calibration = calibrate_oxygen();
}

void loop()
{
    // double x_O2 = 0.209; // Oxygen fraction
    // double T = 293.0; // Temperature in Kelvin
    double x_O2 = measure_oxygen(O2_calibration);
    double T = temperature_measurement();       // Measure the temperature
    double c_mea = speed_measurement(dist_cal); // Speed of sound in m/s
    double hum = humidity_measurement();
    double x_H2O = measure_water(T, hum, pres_atm);

    // calibrate_distance (2, T, x_H2O);

    double x_He = measure_helium(x_O2, x_H2O, c_mea, T);

    // Display the O2, He fractions and Temperature
    displayValues(x_O2, x_He, T, hum);
    serialdisplayValues(c_mea, x_O2, x_He, x_H2O, hum, T);

    delay(500);
}

void displayValues(double x_O2, double x_He, double T, double hum)
{
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("O2: ");
    display.print(x_O2 * 100, 2); // Display O2 fraction in percentage
    display.println("%");

    display.setCursor(0, 22);
    display.print("He: ");
    display.print(x_He * 100, 2); // Display He fraction in percentage
    display.println("%");

    display.setTextSize(1);
    display.setCursor(0, 44);
    display.print("T: ");
    display.print(T - 273.15, 1); // Display temperature in Celsius
    display.print("C  Hu: ");
    display.print(hum * 100, 1); // Display temperature in Celsius
    display.println("%");

    display.display();
}

void serialdisplayValues(double c_mea, double x_O2, double x_He, double x_H2O, double hum, double T)
{
    Serial.print("Speed: ");
    Serial.print(c_mea, 1); // Display measured speed in m/s
    Serial.print("m/s || ");

    Serial.print("O₂: ");
    Serial.print(x_O2 * 100, 2); // Display O2 fraction in percentage
    Serial.print("% || ");

    Serial.print("He: ");
    Serial.print(x_He * 100, 2); // Display He fraction in percentage
    Serial.print("% || ");

    Serial.print("H₂O: ");
    Serial.print(x_H2O * 100, 2); // Display H2O fraction in percentage
    Serial.print("% || ");

    Serial.print("Hum: ");
    Serial.print(hum * 100, 2); // Display Humidity fraction in percentage
    Serial.print("% || ");

    Serial.print("Temp: ");
    Serial.print(T - 273.15, 2); // Display temperature in Celsius
    Serial.println("°C");
}

void Display_Initialise()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
}

// Need to add functionality for O2, temp, buttons, display
