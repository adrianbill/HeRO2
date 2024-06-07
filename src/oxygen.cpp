// Libraries
#include <Arduino.h>        // Basic Library
#include <Wire.h>           // I2C Library
#include <RunningAverage.h> // Running Average Library
#include <ADS1X15.h>        // ADC / Amplifier Library

// Paired Header
#include "oxygen.h"

// Define ADC address (Amplifier)
ADS1115 O2ADC(0x48);

// O2 running average setup
RunningAverage RA_O2_measure(20);
RunningAverage RA_O2_calibration(20);

// Initialiases Analog to Digital Converter for O2 Sensor and clear running average
void O2_Initialise()
{
    if (!O2ADC.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }

    O2ADC.setGain(16);

    RA_O2_measure.clear();
}

// Function to calibration Oxygen
double calibrate_oxygen(double target_O2)
{

    RA_O2_calibration.clear();

    int calCount = 50; // Calibration samples

    for (int i = 0; i <= calCount; i++)
    {
        int16_t reading = O2ADC.readADC_Differential_0_1();
        RA_O2_calibration.addValue(reading);
        delay(10);
    }

    // converts reading to voltage in mV
    double millivolts = ads.toVoltage(RA_O2_calibration.getAverage()) * 1000;

    Serial.print("Voltage: ");
    Serial.print(millivolts, 2);
    Serial.print("mV");
    Serial.print(" ±");
    Serial.print(RA_O2_calibration.getStandardDeviation(), 2);
    Serial.println("mV");

    return target_O2 / millivolts;
}

// Function to measure Oxygen
double measure_oxygen(double O2_cal_factor)
{

    int16_t reading = O2ADC.readADC_Differential_0_1();

    RA_O2_measure.addValue(reading);

    // converts average reading to voltage in mV
    double voltage_meas = O2ADC.tovoltage(RA_O2_measure.getAverage()) * 1000;

    Serial.print("Voltage: ");
    Serial.print(voltage_meas, 2);
    Serial.print("mV");
    Serial.print("  ||  O₂: ");
    Serial.print(voltage_meas * O2_cal_factor, 2);
    Serial.print("% ±");
    Serial.print(O2ADC.tovoltage(RA_O2_measure.getStandardDeviation()) * 1000, 2);
    Serial.println("%");

    return voltage_meas * O2_cal_factor; // Convert mV ADC reading to % O2
}