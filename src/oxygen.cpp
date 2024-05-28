//Libraries
#include <Arduino.h>          // Basic Library
#include <Wire.h>             // I2C Library
#include <Adafruit_GFX.h>     // Graphic Library
#include <RunningAverage.h>   // Running Average Library
#include <Adafruit_ADS1X15.h> // ADC / Amplifier Library

// Paired Header
#include "oxygen.h"


// Define ADC address (Amplifier)
Adafruit_ADS1115 O2ADC;
const float multiplier = 0.0625F; // ADC value/bit for gain of 2
// const double multiplier = 0.0078125; // ADC value/bit for gain of 16

//O2 running average setup
RunningAverage RA_O2(10);


// Initialiases Analog to Digital Converter for O2 Sensor and clear running average
void O2_Initialise()
{
    if (!O2ADC.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }

    O2ADC.setGain(GAIN_TWO); // Set ADC gain

    RA_O2.clear();
}


// Function to calibration Oxygen
double calibrate_oxygen()
{
    // Running Average Setup
    RunningAverage RA_O2C(450);

    RA_O2C.clear();

    int calCount = 500; // Calibration samples

    for (int i = 0; i <= calCount; i++)
    {
        double millivolts = O2ADC.readADC_SingleEnded(0); // Read differental voltage between ADC pins 0 & 1
        RA_O2C.addValue(millivolts);
        delay(10);
    }

    // Compute calValue
    double mv_mea = RA_O2C.getAverage();
    mv_mea = mv_mea * multiplier;

    Serial.print("mv_mea: ");
    Serial.print(mv_mea);
    Serial.print("  cal: ");
    Serial.println(0.209 / mv_mea, 8);


    return (0.209 / mv_mea);

}

// Function to measure Oxygen
double measure_oxygen(double O2_cal)
{

    double millivolts = O2ADC.readADC_SingleEnded(0);
    RA_O2.addValue(millivolts);


    double mv_mea = RA_O2.getAverage();

    mv_mea = mv_mea * multiplier;

    Serial.print("Ave: ");
    Serial.print(mv_mea, 2);
    Serial.print("mV");

    Serial.print("Cal: ");
    Serial.print(O2_cal, 9);

    // Serial.print(" || Std.d: ");
    // Serial.print(RA_O2.getStandardDeviation() * multiplier, 2);
    // Serial.print("mV");

    // Serial.print(" || Max: ");
    // Serial.print(RA_O2.getMaxInBuffer() * multiplier, 2);
    // Serial.println("mV");

    return mv_mea * O2_cal; // Convert mV ADC reading to % O2
}