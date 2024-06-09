// Libraries
#include <Arduino.h>        // Basic Library
#include <Wire.h>           // I2C Library
#include <RunningAverage.h> // Running Average Library
#include <BME280I2C.h>      // Temp/Hum/Pres sensor Library

// Paired Header
#include "environment.h"

// Define Bosch BM$280 sensor
BME280I2C bme;

// Running Average Setup
RunningAverage RA_temperature(10);
RunningAverage RA_humidity(10);
RunningAverage RA_atmpressure(10);

// Initialize HTU21D sensor and clear running averages
void Environment_Initialise()
{
    if (!bme.begin())
    {
        Serial.println("Could not find BME280 sensor!");
        while (1)
            ;
    }

    RA_temperature.clear();
    RA_humidity.clear();
    RA_atmpressure.clear();
}

// Temperature Measurement Function in Kelvin
double temperature_measurement()
{
    double temp_measure = bme.temp() + 273.15;
    RA_temperature.addValue(temp_measure);
    return RA_temperature.getAverage();
}

// Relative Humidity Measurement Function
double humidity_measurement()
{
    double hum_measure = bme.hum() / 100;
    RA_humidity.addValue(hum_measure);
    return RA_humidity.getAverage();
}

// Atmospheric Pressure Measurement Function in kPa
double atmpressure_measurement()
{
    double atmpress_measure = bme.pres() / 10; // reads pressure in hPa and converts to kPa
    RA_atmpressure.addValue(atmpress_measure);
    return RA_atmpressure.getAverage();
}

// Function to calculate the fraction of water in gas using relative humidity
double measure_water(double temperature, double hum, double pres_atm)
{
    double temperature_C = temperature - 273.15; // convert K to C

    // calculate saturation vapour pressure of water kPa
    double pres_sat = 0.61078 * exp((17.27 * (T - 273.15) / ((temperature - 273.15) + 237.3)); // Tetens equation
    return (pres_sat * hum) / pres_atm;                             // calculate x_H20
}