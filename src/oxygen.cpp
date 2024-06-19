// Libraries
#include <Arduino.h>        // Basic Library
#include <Wire.h>           // I2C Library
#include <RunningAverage.h> // Running Average Library
#include <ADS1X15.h>        // ADC / Amplifier Library

// Custom Headers
#include "constants.h"

// Paired Header
#include "oxygen.h"

// Define ADC address (Amplifier)
ADS1115 ads(0x48);

// O2 running average setup
RunningAverage RA_O2_measure(100);
RunningAverage RA_O2_calibration(100);

// Initialiases Analog to Digital Converter for O2 Sensor and clear running average
int O2_Initialise()
{
    if (!ads.begin())
    {
        Serial.println("Failed to initialize ADS.");
        while (1)
            ;
    }

    ads.setGain(16);

    RA_O2_measure.clear();
    return 1;
}

// Function to calibration Oxygen
void calibrate_oxygen()
{

    RA_O2_calibration.clear();

    int calCount = 200; // Calibration samples

    for (int i = 0; i <= calCount; i++)
    {
        int16_t reading = ads.readADC_Differential_0_1();
        RA_O2_calibration.addValue(reading);
        delay(10);
    }

    // converts reading to voltage in mV
    double millivolts = ads.toVoltage(RA_O2_calibration.getAverage()) * 1000;

    // Serial.print("Voltage: ");
    // Serial.print(millivolts, 2);
    // Serial.print("mV");
    // Serial.print(" ±");
    // Serial.print(RA_O2_calibration.getStandardDeviation(), 2);
    // Serial.println("mV");

    O2_calibration = O2_cal_target / millivolts;

    Serial.println("O₂ Calibrated");
}

// Function to measure Oxygen
double oxygen_measurement()
{

    int16_t reading = ads.readADC_Differential_0_1();

    RA_O2_measure.addValue(reading);

    // converts average reading to voltage in mV
    double voltage_meas = ads.toVoltage(RA_O2_measure.getAverage()) * 1000;

    // Serial.print("Voltage: ");
    // Serial.print(voltage_meas, 2);
    // Serial.print("mV");
    // Serial.print("  ||  O₂: ");
    // Serial.print(voltage_meas * O2_cal_factor, 2);
    // Serial.print("% ±");
    // Serial.print(ads.toVoltage(RA_O2_measure.getStandardDeviation()) * 1000 * O2_cal_factor, 2);
    // Serial.println("%");

    return voltage_meas * O2_calibration; // Convert mV ADC reading to % O2
}

// Function to return O2 millivolts
double oxygen_millivolts()
{
    int16_t reading = ads.readADC_Differential_0_1();

    RA_O2_measure.addValue(reading);

    return ads.toVoltage(RA_O2_measure.getAverage()) * 1000;
}

double MOD_calculate(double O2_fraction, double O2_partial_pressure)
{
    return 10 * (O2_partial_pressure/O2_fraction - 1);
}