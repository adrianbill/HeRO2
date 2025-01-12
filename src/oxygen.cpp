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
#include "environment.h"
#include "menu.h"

// Paired Header
#include "oxygen.h"

// Define ADC address (Amplifier)
ADS1115 ads(0x48);

// O2 running average setup
RunningMedian RM_O2_mv(11);
RunningMedian RM_O2_fraction(5);
RunningMedian RM_mv_calibration(200);

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
        RM_O2_mv.clear();
        
        while (!RM_O2_mv.isFull()) RM_O2_mv.add(ads.toVoltage(ads.readADC_Differential_0_1()) * 1000.0); 

        double O2_mv_median = RM_O2_mv.getMedianAverage(5);

        check_button_event();
        
        RM_O2_mv.clear();
        
        return O2_mv_median;
}

// Function to calibration Oxygen
void calibrate_oxygen(double O2_Fraction)
{
        RM_mv_calibration.clear();
        
        for (size_t i = 0; i < 1000; i++)
        {
                RM_mv_calibration.add(ads.readADC_Differential_0_1());
        }

        double voltage_meas_mV = ads.toVoltage(RM_mv_calibration.getMedianAverage(50)) * 1000.0;

        double p_local_kPa = atmpressure_measurement();

        double p_H2O = water_measurement() * p_local_kPa;

        O2_calibration = O2_Fraction * (p_local_kPa - p_H2O) / voltage_meas_mV;

        EEPROM.writeDouble(eeprom_O2_address, O2_calibration);
        EEPROM.commit();

        RM_mv_calibration.clear();

        // Serial.println("O₂ Calibrated");
        
        // Serial.print("Value: ");
        // Serial.print(O2_calibration, 8);
        // Serial.print(" | eeprom: ");
        // Serial.println(EEPROM.readDouble(eeprom_O2_address), 8);
}

// Function to measure Oxygen
double oxygen_measurement(void)
{
        RM_O2_fraction.clear();
        
        while (!RM_O2_fraction.isFull()) RM_O2_fraction.add(oxygen_millivolts() * O2_calibration / atmpressure_measurement());

        check_button_event();

        double O2_Fraction = RM_O2_fraction.getMedianAverage(3);

        RM_O2_fraction.clear();

        return O2_Fraction;
        
        // return oxygen_millivolts() * O2_calibration / p_local_kPa;  
}

double oxygen_stddev(void)
{
        return 0; 
}