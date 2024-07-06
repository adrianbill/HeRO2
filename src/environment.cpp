// Libraries
#include <Arduino.h>	    // Basic Library
#include <BME280I2C.h>	    // Temp/Hum/Pres sensor Library
#include <RunningAverage.h> // Running Average Library
#include <Wire.h>	    // I2C Library

// Paired Header
#include "environment.h"

// Define Bosch BME280 sensor
BME280I2C bme;

// Running Average Setup
RunningAverage RA_temperature(100);
RunningAverage RA_humidity(100);
RunningAverage RA_pressure(100);

// Initialize HTU21D sensor and clear running averages
int Environment_Initialise()
{
	if (!bme.begin()) {
		Serial.println("Could not find BME280 sensor!");
		while (1)
			;
	}

	RA_temperature.clear();
	RA_humidity.clear();
	RA_pressure.clear();

	return 1;
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

// Function to calculate the fraction of water in gas using relative humidity using Tetens equation
double water_measurement()
{
	double temperature_C = temperature_measurement() - 273.15;
	double humidity = humidity_measurement();
	double pressure_total_kPa = atmpressure_measurement();
	double pressure_saturation_kPa = 0.61078 * exp((17.27 * temperature_C) / (temperature_C + 237.3));

	return (pressure_saturation_kPa * humidity) / pressure_total_kPa; // calculate x_H20
}