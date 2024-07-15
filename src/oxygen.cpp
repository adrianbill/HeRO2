// Libraries
#include <ADS1X15.h>	    // ADC / Amplifier Library
#include <Arduino.h>	    // Basic Library
#include <RunningAverage.h> // Running Average Library
#include <Wire.h>	    // I2C Library
#include <math.h>	    // Math Library
#include "EEPROM.h"

// Custom Headers
#include "constants.h"
#include "helium.h"

// Paired Header
#include "oxygen.h"

// Define ADC address (Amplifier)
ADS1115 ads(0x48);

// O2 running average setup
RunningAverage RA_O2_measure(100);
RunningAverage RA_O2_calibration(100);

// Initialises Analog to Digital Converter for O2 Sensor and clear running average
int O2_Initialise(void)
{
        if (!ads.begin()) {
                Serial.println("Failed to initialize ADS.");
                while (1)
                        ;
        }

        ads.setGain(16);

        RA_O2_measure.clear();

        return 1;
}

// Function to calibration Oxygen
void calibrate_oxygen(void)
{
        RA_O2_calibration.clear();

        for (int i = 0; i <= 500; i++) {
                RA_O2_calibration.addValue(ads.readADC_Differential_0_1());
        }

        double voltage_meas_mV = ads.toVoltage(RA_O2_calibration.getAverage()) * 1000;

        O2_calibration = O2_cal_target / voltage_meas_mV;

        EEPROM.writeDouble(eeprom_O2_address, O2_calibration);
        EEPROM.commit();

        Serial.println("O₂ Calibrated");
        
        Serial.print("Value: ");
        Serial.print(O2_calibration, 8);
        Serial.print(" | eeprom: ");
        Serial.println(EEPROM.readDouble(eeprom_O2_address), 8);
}

// Function to measure Oxygen
double oxygen_measurement(void)
{
        double voltage_meas_mV = oxygen_millivolts();

        return voltage_meas_mV * O2_calibration;
}

// Function to return O2 millivolts
double oxygen_millivolts(void)
{
        RA_O2_measure.addValue(ads.readADC_Differential_0_1());

        return ads.toVoltage(RA_O2_measure.getAverage()) * 1000;
}

int MOD_O2_calculate(double O2_fraction, double O2_partial_pressure)
{
        return round(10 * (O2_partial_pressure / O2_fraction - 1));
}

int MOD_density_calculate(double He_fraction, double O2_fraction, double H2O_fraction, double temperature, double density)
{
        double N2_fraction = 1 - (He_fraction + O2_fraction + H2O_fraction);
        double mix_molar_mass_g = 1000 * calculate_molar_mass(He_fraction, O2_fraction, N2_fraction, H2O_fraction);
        double pressure_kPa = (R_gas_constant * temperature * density) / mix_molar_mass_g;
        return round(0.1 * pressure_kPa - 10);
}