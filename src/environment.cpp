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
RunningAverage RA_pressure(10);

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
    RA_pressure.clear();
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
    double pressure_total = bme.pres() / 10; // reads pressure in hPa and converts to kPa
    RA_pressure.addValue(pressure_total);
    return RA_pressure.getAverage();
}

// Function to calculate the fraction of water in gas using relative humidity
double water_measurement(double temperature_K, double hum, double pressure_total)
{
    //convert temperature from K to C
    double temperature_C = temperature_K - 273.15;


    // calculate saturation vapour pressure of water kPa using Tetens equation 
    double pressure_saturation = 0.61078 * exp((17.27 * temperature_C) / (temperature_C + 237.3)); 
    // calculate x_H20
    return (pressure_saturation * hum) / pressure_total;                             
}