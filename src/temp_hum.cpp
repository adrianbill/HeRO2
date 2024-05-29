// Libraries
#include <Arduino.h>          // Basic Library
#include <Wire.h>             // I2C Library
#include <RunningAverage.h>   // Running Average Library
#include <Adafruit_HTU21DF.h> // Temp/Hum sensor Library

// Paired Header
#include "temp_hum.h"

// Temp/Hum sensor info
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

// Running Average Setup
RunningAverage RA_temp(10);
RunningAverage RA_hum(10);

// Initialize HTU21D sensor and clear running averages
void Temp_Initialise()
{
    if (!htu.begin())
    {
        Serial.println("Check circuit. HTU21D not found!");
        while (1)
            ;
    }

    RA_temp.clear();
    RA_hum.clear();
}

// Temperature Measurement Function in Kelvin
double temperature_measurement()
{

    double temp = htu.readTemperature() + 273.15;
    RA_temp.addValue(temp);

    return RA_temp.getAverage();
}

// Relative Humidity Measurement Function
double humidity_measurement()
{

    double hum = htu.readHumidity() / 100;
    RA_hum.addValue(hum);

    return RA_hum.getAverage();
}

// Function to calculate the fraction of water in gas using relative humidity
double measure_water(double T, double hum, double pres_atm)
{
    double T_C = T - 273.15; // convert K to C

    // calculate saturation vapor pressure of water kPa
    double pres_sat = 0.61078 * exp((17.27 * T_C) / (T_C + 237.3)); // Tetens equation
    return (pres_sat * hum) / pres_atm;                             // calculate x_H20
}