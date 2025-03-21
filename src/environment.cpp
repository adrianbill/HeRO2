// Libraries
#include <Arduino.h>	    // Basic Library
#include <BME280I2C.h>	    // Temp/Hum/Pres sensor Library
#include <RunningAverage.h> // Running Average Library
#include <RunningMedian.h>  // Running Median Library
#include <Wire.h>	    // I2C Library
#include <EEPROM.h>

// Custom Headers
#include "constants.h"
#include "menu.h"

// Paired Header
#include "environment.h"

// Define Bosch BME280 sensor
BME280I2C bme;

// Running Average Setup
RunningMedian RM_temperature(5);
RunningMedian RM_humidity(5);
RunningMedian RM_pressure(5);

// Initialize HTU21D sensor and clear running averages
int Environment_Initialise(void)
{
        if (!bme.begin()) {
                Serial.println("Could not find BME280 sensor!");
                while (1)
                        ;
        }

        RM_temperature.clear();
        RM_humidity.clear();
        RM_pressure.clear();

        return 1;
}

// Temperature Measurement Function in Kelvin
double temperature_measurement(void)
{
        double temp_measure = bme.temp() + 273.15 + temperature_calibration;
        RM_temperature.add(temp_measure);

        check_button_event();

        return RM_temperature.getMedianAverage(5);
}

// Temperature Measurement Function in Celcius
double temperature_measurement_raw(void)
{
        check_button_event();

        return bme.temp();
}

void calibrate_temperature(int sign, double offset)
{
        double temp_sign;

        if (sign == 0) temp_sign = 1.0;
        if (sign == 1) temp_sign = -1.0;

        temperature_calibration = temp_sign * offset;

        EEPROM.writeDouble(eeprom_temp_address, temperature_calibration);
        EEPROM.commit();
}

// Relative Humidity Measurement Function
double humidity_measurement(void)
{
        double hum_measure = bme.hum() / 100.0;

        RM_humidity.add(hum_measure);

        check_button_event();

        return RM_humidity.getMedianAverage(5);
}

// Relative Humidity Measurement Function
double humidity_measurement_raw(void)
{
        check_button_event();
        return bme.hum();
}

// Atmospheric Pressure Measurement Function in kPa
double atmpressure_measurement(void)
{
        double pressure_total = bme.pres() / 10.0; // reads pressure in hPa and converts to kPa

        RM_pressure.add(pressure_total);

        check_button_event();

        return RM_pressure.getMedianAverage(5);
}

double atmpressure_measurement_raw(void)
{
        check_button_event();
        return bme.pres() / 10.0;
}

// Function to calculate the fraction of water in gas using relative humidity using the Buck equation
double water_measurement(void)
{
        double temperature_C = temperature_measurement() - 273.15;
        double humidity = humidity_measurement();
        double pressure_total_kPa = atmpressure_measurement();
        // double pressure_saturation_kPa = 0.61078 * exp((17.27 * temperature_C) / (temperature_C + 237.3)); // Tetens Equation 
        double pressure_saturation_kPa = 0.61121 * exp((18.678 - temperature_C / 234.5) * (temperature_C / (257.14 + temperature_C))); // Buck Equation

        check_button_event();

        return (pressure_saturation_kPa * humidity) / pressure_total_kPa; // calculate x_H20
}