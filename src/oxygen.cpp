// Libraries
#include <ADS1X15.h>	    // ADC / Amplifier Library
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <RunningMedian.h>  // Running Median Library
#include <Wire.h>	    // I2C Library
#include <math.h>	    // Math Library
#include <EEPROM.h>         // EEPROM library

// Custom Headers
#include "constants.h"
#include "helium.h"

// Paired Header
#include "oxygen.h"

// Define ADC address (Amplifier)
ADS1115 ads(0x48);

// O2 running average setup
RunningMedian RM_O2_mv(11);
RunningMedian RM_O2_fraction(19);
RunningAverage RM_O2_calibration(500);

// Initialises Analog to Digital Converter for O2 Sensor and clear running average
int O2_Initialise(void)
{
        if (!ads.begin()) {
                Serial.println("Failed to initialize ADS.");
                while (1)
                        ;
        }

        ads.setGain(16);

        RM_O2_mv.clear();

        return 1;
}

// Function to return O2 millivolts
double oxygen_millivolts(void)
{
        for (size_t i = 0; i < 10; i++) {
                RM_O2_mv.add(ads.toVoltage(ads.readADC_Differential_0_1()) * 1000.0); 
        }
        
        return RM_O2_mv.getMedian();
}

// Function to calibration Oxygen
void calibrate_oxygen(void)
{
        RM_O2_calibration.clear();

        for (int i = 0; i <= 1000; i++) {
                RM_O2_calibration.add(ads.readADC_Differential_0_1());
        }

        double voltage_meas_mV = ads.toVoltage(RM_O2_calibration.getAverage()) * 1000.0;

        O2_calibration = O2_cal_target / voltage_meas_mV;

        EEPROM.writeDouble(eeprom_O2_address, O2_calibration);
        EEPROM.commit();

        // Serial.println("O₂ Calibrated");
        
        // Serial.print("Value: ");
        // Serial.print(O2_calibration, 8);
        // Serial.print(" | eeprom: ");
        // Serial.println(EEPROM.readDouble(eeprom_O2_address), 8);
}

// Function to measure Oxygen
double oxygen_measurement(void)
{
        RM_O2_fraction.add(oxygen_millivolts() * O2_calibration);    

        return RM_O2_fraction.getMedian();
}

double oxygen_stddev(void)
{
        return 0; 
}